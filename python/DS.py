from enum import Enum
from ctypes import *
import numpy as np

#
# #_lib = CDLL('libds.so')
#
_lib = CDLL('/usr/local/stow/ds-2017-12-27.15-01-52/lib/libds.so')

_lib.ds_version.restype = None
_lib.ds_version.argtypes = [POINTER(c_int), POINTER(c_int), POINTER(c_int)]

_lib.ds_open.restype = c_void_p
_lib.ds_open.argstype = [c_char_p, c_int, c_int]

_lib.ds_close.restype = None
_lib.ds_close.argstype = [c_void_p]

_lib.ds_column_name.restype = c_char_p
_lib.ds_column_name.argstype = [c_void_p]

_lib.ds_column_type.restype = c_int
_lib.ds_column_type.argstype = [c_void_p]

_lib.ds_column_length.restype = c_int
_lib.ds_column_length.argstype = [c_void_p]

_lib.ds_column_width.restype = c_int
_lib.ds_column_width.argstype = [c_void_p]

_lib.ds_get_column_by_name.restype = c_void_p
_lib.ds_get_column_by_name.argstype = [c_char_p]

_lib.ds_get_column_by_index.restype = c_void_p
_lib.ds_get_column_by_index.argstype = [c_int]


class DS(object):
    class Type(Enum):
        T_INVALID = 0
        T_BOOL = 1
        T_INT8 = 2
        T_INT16 = 3
        T_INT32 = 4
        T_INT64 = 5
        T_UINT8 = 6
        T_UINT16 = 7
        T_UINT32 = 8
        T_UINT64 = 9
        T_FLOAT32 = 10
        T_FLOAT64 = 11
        T_STRING8 = 12
        T_STRING16 = 13
        T_STRING3 = 14

    O_READ = 0x01
    O_WRITE = 0x02
    O_TRUNC = 0x04
    O_CREATE = 0x08
    O_SAFE = 0x10
    O_UNIQUE = 0x20

    def __init__(self, path, mode=O_READ, buff_siz=1024):
        """
        :param path: path to DS
        :param mode: open file mode. Default O_READ
        :param buff_siz: buffer size. Default is 1024
        """
        self.ds = c_void_p(_lib.ds_open(path.encode('ascii'), mode, buff_siz))

    def columns(self):
        """
        Get list of columns
        :return: List of tuples (name, type, shape)
        """
        num = _lib.ds_cols(self.ds)
        result = []
        for k in range(0, num):
            colh = c_void_p(_lib.ds_get_column_by_index(self.ds, k))
            name = _lib.ds_column_name(colh)
            ctyp = _lib.ds_column_type(colh)
            clen = _lib.ds_column_length(colh)
            cwid = _lib.ds_column_width(colh)
            result += [(name, DS.Type(ctyp), (clen, cwid))]
        return result

    def read(self, key, offs=0, num=-1):
        """
        Read column from specified offset

        :param key: int or str. Index or name of the column
        :param offs: offset to read from
        :param num: number of rows to read
        :return: numpy matrix
        """
        if type(key) == str:
            colh = c_void_p(_lib.ds_get_column_by_name(self.ds, key.encode('ascii')))
        elif type(key) == int:
            colh = c_void_p(_lib.ds_get_column_by_index(self.ds, key))
        else:
            raise Exception('key expected to be name or index. Found %s' % str(type(key)))

        clen = _lib.ds_column_length(colh)
        cwid = _lib.ds_column_width(colh)
        ctyp = _lib.ds_column_type(colh)

        if num == -1:
            num = clen - offs

        csiz = num * cwid
        if DS.Type(ctyp) == DS.Type.T_FLOAT64:
            buff = c_double * csiz
        else:
            raise Exception('unsupported type of column')

        ptr = buff()
        res = _lib.ds_column_read(colh, offs, num, ptr)

        if res != num:
            raise Exception('Read ' + str(res) + ' rows from ' + str(num))

        data = np.frombuffer(cast(ptr, POINTER(buff)).contents)
        return data.reshape(num, cwid)

    def __getitem__(self, key):
        """
        Reads content of the column indexed by key

        :param key: int or str
        :return: numpy matrix
        """
        if type(key) == tuple:
            return tuple(self.read(i, 0, -1) for i in key)
        else:
            return self.read(key, 0, -1)

    def close(self):
        """
        Closes DS

        :return: None
        """
        _lib.ds_close(self.ds)

    @staticmethod
    def version():
        """
        Current version of DS

        :return: Tuple (major, minor, build)
        """
        major = pointer(c_int(1))
        minor = pointer(c_int(1))
        build = pointer(c_int(1))
        _lib.ds_version(major, minor, build)
        return major[0], minor[0], build[0]
