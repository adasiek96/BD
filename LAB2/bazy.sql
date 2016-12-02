1.1.
SELECT job, COUNT(*), ROUND(AVG(sal),2)
	FROM emp
	GROUP BY job;

1.2.1.
SELECT deptno, SUM(com) FROM
   (SELECT *, CASE
    WHEN comm IS NOT NULL THEN 1  
    ELSE 0 END AS com FROM emp)
GROUP BY deptno;
1.2.2. (niepełne)
SELECT deptno, COUNT(*)
	FROM emp
	WHERE comm IS NOT NULL
	GROUP BY deptno;

1.3.1.
SELECT job, MIN(sal), MAX(sal)
	FROM emp WHERE sal >= 1000
	GROUP BY job HAVING COUNT(*) >= 3;
1.3.2.
SELECT job, MIN(sal), MAX(sal)
	FROM emp WHERE job IN
		(SELECT job
			FROM emp
			WHERE sal >= 1000
			GROUP BY job HAVING COUNT(*) >= 3)
	GROUP BY job;

1.4.
SELECT deptno, job
	FROM emp
	GROUP BY deptno, job
	HAVING MAX(sal) - MIN(sal) > 300;

1.5.1.
SELECT AVG(sal)
	FROM emp
	GROUP BY deptno
	HAVING COUNT(*) != COUNT(mgr);
1.5.2.
SELECT AVG(sal)
	FROM emp WHERE deptno IN
		(SELECT deptno
			FROM emp
			WHERE mgr IS NULL)
	GROUP BY deptno;

1.6.1.
SELECT mgr
	FROM emp
	GROUP BY mgr
	HAVING COUNT(DISTINCT deptno) > 1;
1.6.2.
SELECT mgr
	FROM (SELECT mgr, deptno
		FROM emp
		WHERE mgr IS NOT NULL
		GROUP BY deptno, mgr)
	GROUP BY mgr
	HAVING COUNT(*) > 1;

1.7.
SELECT ename, sal
	FROM emp
	GROUP BY deptno, job
	HAVING COUNT(*) = 1
	ORDER BY sal;


2.1.
SELECT deptno
	FROM dept
	WHERE deptno NOT IN
		(SELECT deptno FROM emp);
	
2.2.
SELECT ename
	FROM (SELECT ename, MIN(hiredate) AS date FROM emp)
	WHERE date IS NOT NULL;

2.3.
SELECT A.ename,
		(SELECT B.ename
			FROM emp B
			WHERE A.mgr = B.empno) MGR_NAME
	FROM emp A;

2.4.
SELECT SUM(ile) FROM
	(SELECT job, MAX(sal) * COUNT(*) - SUM(sal) AS ile
		FROM emp
		GROUP BY job);

2.5.
SELECT job, MAX(ile) FROM
	(SELECT job, deptno, COUNT(*) AS ile FROM emp
		GROUP BY job, deptno)
	GROUP BY job;

2a.
SELECT *
	FROM emp
	WHERE comm > (SELECT MIN(sal) FROM emp);

3.1.1.
SELECT DISTINCT loc
	FROM emp NATURAL JOIN dept
	WHERE job = 'SALESMAN';
3.1.2.
SELECT DISTINCT loc
	FROM emp, dept
	WHERE job = 'SALESMAN' AND emp.deptno = dept.deptno;

3.2.1.
SELECT A.ename, B.ename, loc
	FROM (emp A LEFT JOIN emp B ON A.mgr = B.empno)
		LEFT JOIN dept ON dept.deptno = B.deptno;
3.2.2. (niepełne)
SELECT A.ename, B.ename, loc
	FROM emp A, emp B, dept
	WHERE B.deptno = dept.deptno AND A.mgr = B.empno;

3.3.1.
SELECT loc, COUNT(emp.deptno)
	FROM dept NATURAL LEFT JOIN emp
	GROUP BY loc;
3.3.2. (niepełne)
SELECT loc, COUNT(*)
	FROM dept, emp
	WHERE dept.deptno = emp.deptno
	GROUP BY loc;

3.4.
SELECT deptno
	FROM dept
	WHERE deptno NOT IN
		(SELECT deptno
			FROM dept NATURAL JOIN emp);

3.5.
SELECT A.ename, A.sal, COUNT(B.sal) + 1
	FROM emp A LEFT JOIN emp B ON A.sal < B.sal
	GROUP BY A.ename
	ORDER BY A.sal DESC;
	
3.6.
SELECT ename, MIN(hiredate)
	FROM emp;


3a.1.
SELECT A.ename, CASE WHEN SUM(B.sal) IS NOT NULL THEN SUM(B.sal) + A.sal ELSE A.sal END
	FROM emp A LEFT JOIN emp B ON B.mgr = A.empno
	GROUP BY A.ename;

3a.2.1.
SELECT ename
	FROM emp
	ORDER BY ename LIMIT 3;


4a.
UPDATE emp
	SET sal = 1.1 * sal
	WHERE CAST (strftime('%w', hiredate) AS INTEGER) = 1
		OR CAST (strftime('%w', hiredate) AS INTEGER) = 2;
UPDATE emp
	SET sal = 1.2 * sal
	WHERE CAST (strftime('%w', hiredate) AS INTEGER) = 3;
