# coding: utf-8
import time, datetime

# 接收的参数可以为：1. 时间戳字符串 2. datetime, 3. None
def unixtime_fromtimestamp(dt = None): 
    if dt and isinstance(dt, datetime.datetime) :  return int(time.mktime(dt.timetuple()))
    elif dt and isinstance(dt, basestring) :  return int(time.mktime(time.strptime(dt, '%Y-%m-%d %H:%M:%S')))
    else: return int(time.mktime(datetime.datetime.now().timetuple()))

# value为传入的值为时间戳(整形)，如：1332888820
def timestamp_fromunixtime(value=None, format=None):  
    return time.strftime(format if format else '%Y-%m-%d %H:%M:%S'  , time.localtime(value if value else unixtime_fromtimestamp()))

def timestamp_fromunixtime2(value=None, format=None):  
    return time.strftime(format if format else '%Y%m%d_%H%M%S'  , time.localtime(value if value else unixtime_fromtimestamp()))


if __name__ == "__main__":
#     print unixtime_fromtimestamp()
    print unixtime_fromtimestamp(datetime.datetime.today())
    
