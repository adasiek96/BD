import sys
import os
import random
import sqlite3
import json
import requests

sess = requests.Session()
adapter = requests.adapters.HTTPAdapter(max_retries = 20)
sess.mount('https://', adapter)

# p0=nr, p1=name, p2=bus_stop_id, p3=bus_stop_nr, p4=line
def parse(p0, p1, p2, p3, p4):

    # TRYB
    nr = p0

    if (nr != 1 and nr != 2 and nr != 3):
        sys.exit(1)

    DB_FILE = ''
    
    #DB_FILE = str(random.randint(0, 1000))
    #DB_FILE += "_"

    # ZMIENNE
    if (nr == 1):
        name = p1
        DB_FILE += name
    elif (nr == 2):
        bus_stop_id = p2
        bus_stop_nr = p3
        DB_FILE += '__' + bus_stop_id + '_' + bus_stop_nr
    elif (nr == 3):
        bus_stop_id = p2
        bus_stop_nr = p3
        line = p4
        DB_FILE += 'tmp/' + '__' + bus_stop_id + '_' + bus_stop_nr
    
    # STAŁE
    LINK = 'https://api.um.warszawa.pl/api/action/dbtimetable_get/?'
    API_KEY = '&apikey=' # add your own API key after '='

    if (nr == 1):
        ID_1 = 'id=b27f4c17-5c50-4a5b-89dd-236b282bc499'
        NAME = '&name='
        LINK += ID_1 + NAME + name + API_KEY
    else:
        BUS_STOP_ID = '&busstopId='
        BUS_STOP_NR = '&busstopNr='
    if (nr == 2):
        ID_2 = 'id=88cd555f-6f31-43ca-9de4-66c479ad5942'
        LINK += ID_2 + BUS_STOP_ID + bus_stop_id + BUS_STOP_NR + bus_stop_nr + API_KEY
    if (nr == 3):
        ID_3 = 'id=e923fa0e-d96c-43f9-ae6e-60518c9f3238'
        LINE = '&line='
        if (len(line) > 1 and line[1] == '_'):
            LINK += ID_3 + BUS_STOP_ID + bus_stop_id + BUS_STOP_NR + bus_stop_nr + LINE + line[0] + '-' + line[2] + API_KEY
        else:
            LINK += ID_3 + BUS_STOP_ID + bus_stop_id + BUS_STOP_NR + bus_stop_nr + LINE + line + API_KEY

    JSON = requests.get(LINK).json()    
    size = len(JSON["result"])
    t = []
    #print(LINK)

    # METODA NR 1
    def method_1():
        for i in range(size):
            t1 = JSON["result"][i]["values"][0]["value"]
            t2 = JSON["result"][i]["values"][1]["value"]
            t.append([t1, t2])

        k1 = JSON["result"][0]["values"][0]["key"]
        k2 = JSON["result"][0]["values"][1]["key"]

        command = 'CREATE TABLE' + str(DB_FILE) + ' (' + k1 + ', ' + k2 + ')'
        c = conn.cursor()
        c.execute(command)
        for i in range(size):
            command = 'INSERT INTO' + DB_FILE + ' VALUES (' + t[i][0]+ ', ' + t[i][1] + ')'
            c.execute(command)
        conn.commit()
        conn.close()

    # METODA NR 2
    def method_2():
        conn = sqlite3.connect('tmp/linie_na_przystankach.db')
        for i in range(size):
            t1 = JSON["result"][i]["values"][0]["value"]
            t.append(t1)
        k1 = JSON["result"][i]["values"][0]["key"]
        command = 'CREATE TABLE tabela (' + k1 + ')'
        c = conn.cursor()
        c.execute(command)
        for i in range(size):
            c.execute('INSERT INTO tabela VALUES (?)', (t[i],))
        c.execute('ALTER TABLE tabela RENAME TO ' + DB_FILE)
        conn.commit()
        conn.close()

    # METODA NR 3
    def method_3():
        if (JSON["result"][0] != '[' and "values" not in JSON["result"][0]):
            print(DB_FILE + " (nieprawidłowa godzina) [linia: " + line + "]")
            return
        conn = sqlite3.connect(DB_FILE + '.db')
        for i in range(size):
            t1 = JSON["result"][i]["values"][0]["value"]
            t2 = JSON["result"][i]["values"][1]["value"]
            t3 = JSON["result"][i]["values"][2]["value"]
            t4 = JSON["result"][i]["values"][3]["value"]
            t5 = JSON["result"][i]["values"][4]["value"]
            t6 = JSON["result"][i]["values"][5]["value"]
            t.append([t1, t2, t3, t4, t5, t6])

        k1 = JSON["result"][0]["values"][0]["key"]
        k2 = JSON["result"][0]["values"][1]["key"]
        k3 = JSON["result"][0]["values"][2]["key"]
        k4 = JSON["result"][0]["values"][3]["key"]
        k5 = JSON["result"][0]["values"][4]["key"]
        k6 = JSON["result"][0]["values"][5]["key"]

        command = 'CREATE TABLE tabela (' + k1 + ', ' + k2 + ', ' + k3 + ', ' + k4 + ', ' + k5 + ', ' + k6 + ')'
        c = conn.cursor()
        c.execute(command)
        for i in range(size):
            c.execute('INSERT INTO tabela VALUES (?, ?, ?, ?, ?, ?)', (t[i][0], t[i][1], t[i][2], t[i][3], t[i][4], t[i][5]))
        c.execute('ALTER TABLE tabela RENAME TO _' + line)
        conn.commit()
        conn.close()

    # WYWOŁANIE ODPOWIEDNIEJ METODY
    
    if (JSON["result"] == []):
        print(DB_FILE + " (pusty)")
    else:
        if (nr == 1):
            method_1()
        elif (nr == 2):
            method_2()
        elif (nr == 3):
            method_3()
        print(DB_FILE)


z = open('przystanki.txt', 'r')
lista = []
cols = ['', '']
for line in z.readlines():
    tmp = line.split(',')
    if (cols[1] != tmp[1]):
        lista.append(tmp)
    cols = tmp
z.close()

for i in range(len(lista)):
    lista[i][1] = lista[i][1].strip('\n')

for i in range(len(lista)):
    parse(2, '', lista[i][0], '01', '')
    parse(2, '', lista[i][0], '02', '')


conn = sqlite3.connect("tmp/linie_na_przystankach.db")
c = conn.cursor()
command = "SELECT tbl_name FROM sqlite_master WHERE type='table'"
result = c.execute(command)
res = result.fetchall()
conn.close()

list_of_tables = []
for i in range(len(res)):
    list_of_tables.append(res[i][0])

#print(list_of_tables)

#parse(3, '', '1018', '01', 'N03')


for i in range(len(list_of_tables)):
    bus_stop_id = ''
    bus_stop_id += list_of_tables[i][2] +list_of_tables[i][3] + list_of_tables[i][4] + list_of_tables[i][5]
    bus_stop_nr = ''
    bus_stop_nr = list_of_tables[i][7] + list_of_tables[i][8]
    _conn = sqlite3.connect('tmp/linie_na_przystankach.db')
    _c = _conn.cursor()
    command = 'SELECT COUNT(linia) FROM ' + list_of_tables[i]
    _result = _c.execute(command)
    _res = _result.fetchall()
    size = _res[0][0]
    command = 'SELECT linia FROM ' + list_of_tables[i]
    _result_2 = _c.execute(command)
    _res_2 = _result_2.fetchall()
    _conn.close()
    for j in range(size):
        if (len(_res_2[j][0]) > 1 and _res_2[j][0][1] == '-'):
            temp_str = ''
            temp_str = _res_2[j][0][0] + '_' + _res_2[j][0][2]
            parse(3, '', bus_stop_id, bus_stop_nr, temp_str)
        else:
            parse(3, '', bus_stop_id, bus_stop_nr, _res_2[j][0])

print("Zakończono powodzeniem!")

ff = open('status.txt', 'w')
ff.write("Zakończono powodzeniem!")
