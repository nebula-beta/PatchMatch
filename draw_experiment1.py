#!/usr/bin/python2.7
#coding:utf-8


import numpy as np
import matplotlib.pyplot as plt


# spatial propagation & plane refinemnt & view propagation & post-processing(fill invalid pixel and weighted median filter)

x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

y10_left = np.array([12.673587,10.535477, 10.632952, 10.839293, 10.472181, 10.808279, 10.734857, 10.936769, 10.820938, 10.701943])
y11_left = np.array([19.331604,12.525476, 11.205772, 10.956389, 11.170327, 10.786758, 10.614595, 10.893727, 11.081714, 10.970947])
y12_left = np.array([19.276537,12.712830, 11.239319, 11.314640, 11.314640, 11.379201, 11.162732, 11.079183, 11.101969, 10.969049])

y10_right = np.array([12.897652,11.349453, 11.453257, 11.308944, 11.496297, 11.564023, 11.510855, 11.528578, 11.576682, 11.389328])
y11_right = np.array([19.151213,12.865371, 12.207102, 11.872904, 11.918476, 11.910248, 11.635547, 11.827331, 11.808975, 11.805178])
y12_right = np.array([21.111464,13.859738, 12.373568, 12.121654, 12.004558, 12.105829, 12.021646, 11.829863, 11.974809, 11.945060])



def draw(fig_name, x, ys, names):
    plt.figure(num = fig_name)
    plt.plot(x, ys[0], marker='o', color = 'red', mec='r', mfc='w', label= names[0])
    plt.plot(x, ys[1], marker='o', color = "lime", mec='r', mfc='w', label= names[1])
    plt.plot(x, ys[2], marker='o', color = "blue", mec='r', mfc='w', label= names[2])
    plt.legend()  # 让图例生效

    plt.xlim(0, 11)  # 限定横轴的范围
    plt.xlabel("iteration") #X轴标签
    plt.ylabel("bad error 1.0") #Y轴标签


fig1 = plt.figure(num = "fig1")
fig2 = plt.figure(num = "fig2")

draw("fig1", x, [y10_left, y11_left, y12_left], ["raw", "rows cols sweep", "red black"])

draw("fig2", x, [y10_right, y11_right, y12_right], ["raw", "rows cols sweep", "red black"])
plt.show()
