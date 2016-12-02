2.1.
SELECT * FROM emp WHERE job = 'CLERK';

2.2.
SELECT ename FROM emp WHERE mgr IS NULL;

2.3.
SELECT loc FROM dept;

2.4.
SELECT empno FROM emp WHERE mgr IS NOT NULL ORDER BY empno DESC;

2.5
SELECT ename, sal, job FROM emp WHERE (ename LIKE 'A%' AND sal > 1500 AND sal < 2000) OR job = 'ANALYST';

2.6.
SELECT *, mgr > empno AS 'starszy' FROM emp WHERE job IS NOT NULL;

2.7.
SELECT *, CASE WHEN mgr > empno THEN 'TAK' ELSE 'NIE' END AS 'starszy' FROM emp WHERE job IS NOT NULL;


3.1.1.
INSERT INTO dept VALUES (50, 'IT', 'WARSAW');
3.1.2.
INSERT INTO dept (deptno, dname, loc) VALUES (50, 'IT', 'WARSAW');

3.2.
INSERT INTO emp (ename, job, sal, deptno) VALUES ('ADAM', 'INFORMATICIAN', 2000, 50);

3.3.
UPDATE emp SET sal = 1.23 * sal WHERE ename = 'ADAM';

3.4.1.
DELETE FROM emp WHERE sal > 2460;
3.4.2.
DELETE FROM emp WHERE sal > (SELECT sal FROM emp WHERE ename = 'ADAM');

3.5.
INSERT INTO emp (empno, ename, job, sal, mgr, hiredate, deptno)
   SELECT 8015 AS empno, ename AS ename, job AS job, 0.5 * sal as sal, mgr AS mgr, hiredate AS hiredate, deptno AS deptno
      FROM emp WHERE ename = 'MILLER';
UPDATE emp SET sal = 0.5 * sal WHERE ename = 'MILLER' AND empno <> 8015;
