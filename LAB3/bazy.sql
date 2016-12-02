1.1.1.
SELECT osoba.imie, osoba.nazwisko, COUNT(zwierze.idzwierzecia) AS ilezwierzat
   FROM (osoba LEFT JOIN posiadanie ON osoba.imie = posiadanie.imiewlasciciela)
      LEFT JOIN zwierze ON posiadanie.idzwierzecia = zwierze.idzwierzecia AND zwierze.datasmierci IS NULL
   GROUP BY osoba.imie;
1.1.2.
SELECT osoba.imie, osoba.nazwisko, COUNT(zwierze.idzwierzecia) - COUNT(zwierze.datasmierci) AS ilezwierzat
   FROM (osoba LEFT JOIN posiadanie ON osoba.imie = posiadanie.imiewlasciciela)
      LEFT JOIN zwierze ON posiadanie.idzwierzecia = zwierze.idzwierzecia
   GROUP BY osoba.imie;

1.2.
SELECT gatunek, MAX(liczba)
   FROM (SELECT gatunek, COUNT(*) AS liczba FROM zwierze GROUP BY imie, gatunek) 
   GROUP BY gatunek;

1.3.
SELECT A.mama, B.nazwisko, COUNT(A.tata), COUNT(*) - COUNT(A.tata)
	FROM osoba A JOIN osoba B ON A.mama = B.imie
	GROUP BY A.mama;

1.4.1
SELECT DISTINCT zwierze.imie
	FROM ((osoba A JOIN osoba B ON A.imie = B.mama)
		JOIN posiadanie ON B.imie = posiadanie.imiewlasciciela)
			JOIN zwierze ON posiadanie.idzwierzecia = zwierze.idzwierzecia
	WHERE (A.nazwisko = 'Makota' OR B.nazwisko = 'Makota') AND zwierze.gatunek = 'kot';
1.4.2.
SELECT DISTINCT Z.imie
	FROM osoba A, osoba B, posiadanie P, zwierze Z
	WHERE A.nazwisko = 'Makota' AND (A.imie = B.imie OR A.imie = B.mama)
		AND B.imie = P.imiewlasciciela AND P.idzwierzecia = Z.idzwierzecia AND gatunek = 'kot';

1.5.
SELECT imie, nazwisko, MAX(ile) FROM
(SELECT B.tata AS imie, A.nazwisko AS nazwisko, B.imie, zwierze.imie, COUNT(B.imie) AS ile
	FROM ((osoba B LEFT JOIN osoba A ON A.imie = B.tata)
		JOIN posiadanie ON B.imie = posiadanie.imiewlasciciela)
			JOIN zwierze ON posiadanie.idzwierzecia = zwierze.idzwierzecia
	WHERE B.tata IS NOT NULL AND zwierze.datasmierci IS NULL
	GROUP BY B.imie);

1.6.
SELECT osoba.imie, CASE WHEN zwierze.imie IS NULL THEN '-' ELSE zwierze.imie END
	FROM (osoba LEFT JOIN posiadanie ON osoba.imie = posiadanie.imiewlasciciela)
			LEFT JOIN zwierze ON posiadanie.idzwierzecia = zwierze.idzwierzecia
	WHERE zwierze.datasmierci IS NULL
	GROUP BY osoba.imie;

1.7.
SELECT zwierze.imie, zwierze.gatunek, COUNT(Z.gatunek) FROM zwierze, zwierze Z
	WHERE zwierze.idzwierzecia NOT IN (SELECT idzwierzecia FROM posiadanie)
		AND Z.gatunek = zwierze.gatunek
	GROUP BY zwierze.idzwierzecia;

1.8.
SELECT COUNT(DISTINCT tata)
	FROM (zwierze NATURAL JOIN posiadanie)
		JOIN osoba ON posiadanie.imiewlasciciela = osoba.imie
	WHERE zwierze.gatunek = 'kot';

1.9.
SELECT zwierze.imie, posiadanie.imiewlasciciela, dataprzygarniecia,
		(SELECT COUNT(P.dataprzygarniecia) + 1 FROM posiadanie P
			WHERE P.dataprzygarniecia < posiadanie.dataprzygarniecia
				AND P.imiewlasciciela = posiadanie.imiewlasciciela)
	FROM zwierze NATURAL JOIN posiadanie
	ORDER BY posiadanie.imiewlasciciela, dataprzygarniecia;

1.10.
SELECT B.imie
	FROM (osoba A JOIN osoba B ON (A.imie = B.mama OR A.imie = B.tata))
		JOIN (posiadanie NATURAL JOIN zwierze) ON A.imie = posiadanie.imiewlasciciela
	WHERE datasmierci IS NOT NULL
	ORDER BY datasmierci DESC LIMIT 1;
