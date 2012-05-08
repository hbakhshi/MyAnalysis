class RowObject(dict):
    """ allows access to dict via attributes as keys """

    def __init__(self, *l, **kw):
        dict.__init__(self, *l, **kw)

    def __setattr__(self, k, v):
        dict.__setitem__(self, k, v)

    def __getattr__(self, k):
        return self.get(k)


class Table(object):
    """ represents table and set operations on tables """

    def __init__(self, __PrintSum=True,__PrintHeader=True, rows=[]):
        self.rows = rows[:]
        self.PrintSum = __PrintSum
        self.PrintHeader = __PrintHeader

    def append(self, obj):
        self.rows.append(obj)

    def __iter__(self):
        return iter(self.rows)

    def __getattr__(self, column):
        """ constructs intermediate table when accessing t.column """
        return ITable(self.rows, column)

    def __and__(self, other):
        """ computes intersection of two tables """
        tmp = [ r for r in self.rows if r in other.rows ]
        return Table(tmp)

    def __or__(self, other):
        """ computes union of two tables """
        tmp = self.rows[:]
        tmp.extend(other.rows[:]) # use copys of lists !
        return Table(tmp)

    def GetSumRow(self):
        ret = RowObject()
        ret[ sorted(self.rows[0].keys())[0] ] = ''
        for key in sorted(self.rows[0].keys())[1:]:
            ret[key] = sum([r[key] for r in self.rows])
        return ret

    def __str__(self):
        """ quite stupid, just for demonstration purposes """
        txt = ''
        if self.PrintHeader:
            txt = "  |" + "|".join(sorted(self.rows[0].keys())).expandtabs()
            txt += "\n"
        txt += "|-"
        for r in self.rows:
            txt += "\n|"
            txt += "|".join([str(round(r[key] , 2)  if isinstance(r[key], (int, long, float, complex)) else r[key]) for key in sorted(self.rows[0].keys())])
        txt += "\n|-"
        if self.PrintSum:
            txt += "\n"
            sumRow = self.GetSumRow()
            txt += "| |" + "|".join( [str(round(sumRow[key] , 2)  if isinstance(sumRow[key], (int, long, float, complex)) else sumRow[key]) for key in sorted(self.rows[0].keys())[1:]] )

        return txt



class ITable(object):
    """ intermediate table for storing fixed column name internally """

    def __init__(self, rows, column):
        self.rows=rows[:]
        self.column=column

    def _extract(self, fun):
        return Table([ row for row in self.rows if fun(row.get(self.column)) ]) 

    def __le__(self, limit):
        return self._extract(lambda v: v<=limit)

    def __ge__(self, limit):
        return self._extract(lambda v: v>=limit)

    def __eq__(self, other):
        return self._extract(lambda v: v==other)

