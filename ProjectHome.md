mapper addons for lua in mushclient

mud客户端muchclient的地图插件，提供lua接口

算法：

一个rooms数组，储存room name,出口信息。固定信息，open()时生成

一个path类型，分为3块

1.普通路径，roads,open()时生成
3.特殊tag才能通过的路径，settags()时生成

path的结构:
{
content:命令
to:到什么地方
from:从身地方来
next :同一类的下一个path,清除数据时使用。＊
delay:这条路径要耽误多少时间。
}



一正一反量两个个walkedroom数组，每次计算路径时生成，为空则没走过，否则为上次走到这个的path的指针。
从起点和终点双向出发可以大量缩小计算量(1/4)。