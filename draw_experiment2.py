#!/usr/bin/python2.7
#coding:utf-8


import numpy as np
import matplotlib.pyplot as plt


x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

# just spatial propagation
y1_left = np.array([57.328945,41.184887, 40.714603, 37.908096, 38.082157, 36.266220, 39.192352, 36.218117, 38.747391, 36.591557])
y2_left = np.array([85.214256,61.762138, 50.138615, 43.305271, 39.008163, 36.921959, 35.660484, 34.281284, 34.640167, 33.772392])
y3_left = np.array([79.115768,54.756630, 42.790051, 39.696182, 35.442116, 33.806568, 33.804672, 31.038673, 32.590038, 31.914045])

y1_right = np.array([52.657764,42.882462, 40.353188, 39.339199, 39.856953, 39.907589, 36.244064, 39.306286, 36.499146, 39.091084])
y2_right = np.array([85.317429,61.926071, 51.850117, 43.708462, 42.741314, 39.330338, 35.345909, 36.476360, 33.024876, 35.788342])
y3_right = np.array([78.872711,55.420597, 44.393948, 39.580353, 37.139061, 35.078171, 35.211090, 33.308437, 31.765301, 33.135643])



# spatial propagation & plane refinement
y4_left = np.array([17.631496,14.935756, 14.727514, 14.710424, 14.871827, 14.868663, 14.939553, 14.958542, 14.921830, 14.989556])
y5_left = np.array([40.070892,19.865814, 16.344072, 15.668714, 15.428824, 15.342743, 15.328818, 15.357301, 15.308564, 15.401608])
y6_left = np.array([67.622002,35.344009, 22.007723, 18.078993, 16.842838, 16.425091, 16.390911, 16.278246, 16.232040, 16.126337])

y4_right = np.array([19.533514,15.875689, 15.859232, 15.863029, 15.851004, 15.788974, 15.893411, 15.819356, 15.983923, 15.937717])
y5_right = np.array([42.571049,21.907715, 17.747326, 16.792202, 16.576998, 16.550415, 16.594088, 16.351667, 16.330780, 16.452307])
y6_right = np.array([67.444145,35.240837, 23.595163, 19.508829, 18.049877, 17.346668, 17.307425, 17.135262, 17.217545, 17.009937])

# spatial propagation & plane refinemnt & view propagation
y7_left = np.array([16.327616,14.739540, 14.590164, 14.633204, 14.817393, 14.800937, 14.753466, 14.918666, 14.961074, 14.954744])
y8_left = np.array([31.271599,17.496044, 15.664915, 15.256661, 15.201595, 15.017406, 15.170581, 15.249700, 15.302235, 15.149693])
y9_left = np.array([43.197037,20.583582, 16.965631, 16.130768, 15.962402, 15.897208, 15.828850, 15.894044, 15.919995, 15.967466])

y7_right = np.array([17.310589,15.560478, 15.466802, 15.537060, 15.698462, 15.764922, 15.756061, 15.736439, 15.825052, 15.806697])
y8_right = np.array([30.661434,18.720804, 16.585859, 16.239002, 16.100386, 16.126970, 16.049749, 16.135199, 16.285841, 16.124437])
y9_right = np.array([35.853535,19.679726, 17.268814, 16.826382, 16.544718, 16.647257, 16.554213, 16.611811, 16.604216, 16.595354])

# spatial propagation & plane refinemnt & view propagation & post-processing(fill invalid pixel and weighted median filter)
y10_left = np.array([12.673587,10.535477, 10.632952, 10.839293, 10.472181, 10.808279, 10.734857, 10.936769, 10.820938, 10.701943])
y11_left = np.array([19.331604,12.525476, 11.205772, 10.956389, 11.170327, 10.786758, 10.614595, 10.893727, 11.081714, 10.970947])
y12_left = np.array([19.276537,12.712830, 11.239319, 11.314640, 11.314640, 11.379201, 11.162732, 11.079183, 11.101969, 10.969049])

y10_right = np.array([12.897652,11.349453, 11.453257, 11.308944, 11.496297, 11.564023, 11.510855, 11.528578, 11.576682, 11.389328])
y11_right = np.array([19.151213,12.865371, 12.207102, 11.872904, 11.918476, 11.910248, 11.635547, 11.827331, 11.808975, 11.805178])
y12_right = np.array([21.111464,13.859738, 12.373568, 12.121654, 12.004558, 12.105829, 12.021646, 11.829863, 11.974809, 11.945060])





def drawCom(fig_name, title, x, ys, names):
    plt.figure(num = fig_name)
    plt.plot(x, ys[0], marker='o', color = 'red', mec='r', mfc='w', label= names[0])
    plt.plot(x, ys[3], marker='*', color = 'red', mec='r', mfc='w', label= names[3])

    plt.plot(x, ys[1], marker='o', color = "lime", mec='r', mfc='w', label= names[1])
    plt.plot(x, ys[4], marker='*', color = "lime", mec='r', mfc='w', label= names[4])

    plt.plot(x, ys[2], marker='o', color = "blue", mec='r', mfc='w', label= names[2])
    plt.plot(x, ys[5], marker='*', color = "blue", mec='r', mfc='w', label= names[5])
    plt.legend()  # 让图例生效

    plt.xlim(0, 11)  # 限定横轴的范围
    plt.xlabel("iteration") #X轴标签
    plt.ylabel("bad error 1.0") #Y轴标签
    # plt.title(title) #标题



fig1 = plt.figure(num = "fig1")
drawCom("fig1", "left disparity iter", x, [y1_left, y2_left, y3_left, y4_left, y5_left, y6_left],
        ["patch_match", "rows_cols", "red_black", "patch_match_refine", "rows_cols_refine", "red_black_refine"])

fig2 = plt.figure(num = "fig2")
drawCom("fig2", "right disparity iter", x, [y1_right, y2_right, y3_right, y4_right, y5_right, y6_right],
        ["patch_match", "rows_cols", "red_black", "patch_match_refine", "rows_cols_refine", "red_black_refine"])

fig3 = plt.figure(num = "fig3")
drawCom("fig3", "left disparity iter", x, [y4_left, y5_left, y6_left, y7_left, y8_left, y9_left],
        ["patch_match_refine", "rows_cols_refine", "red_black_refine", "patch_match_refine_view_prop", "rows_cols_refine_view_prop", "red_black_refine_view_prop"])

fig4 = plt.figure(num = "fig4")
drawCom("fig4", "right disparity iter", x, [y4_right, y5_right, y6_right, y7_right, y8_right, y9_right],
        ["patch_match_refine", "rows_cols_refine", "red_black_refine", "patch_match_refine_view_prop", "rows_cols_refine_view_prop", "red_black_refine_view_prop"])

fig5 = plt.figure(num = "fig5")
drawCom("fig5", "left disparity iter", x, [y7_left, y8_left, y9_left, y10_left, y11_left, y12_left],
        ["patch_match_refine_view_prop", "rows_cols_refine_view_prop", "red_black_refine_view_prop", "patch_match_refine_view_prop_post_process", "rows_cols_refine_view_prop_post_process", "red_black_refine_view_prop_post_process"])

drawCom("fig6", "right disparity iter", x, [y7_right, y8_right, y9_right, y10_right, y11_right, y12_right],
        ["patch_match_refine_view_prop", "rows_cols_refine_view_prop", "red_black_refine_view_prop", "patch_match_refine_view_prop_post_process", "rows_cols_refine_view_prop_post_process", "red_black_refine_view_prop_post_process"])

plt.show()
