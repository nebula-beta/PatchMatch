#!/usr/bin/python2.7
#coding:utf-8



def read_data_from_file(file_name):
    fp = open(file_name)
    lines = fp.readlines()

    left_datas = []
    right_datas = []

    cnt = 0
    for line in lines:
        if line[0] == '[':
            cnt += 1
            if cnt == 1:
                line = line[1 : -2].split(',')
                line = map(float, line)
                left_datas.append(line)
            elif cnt == 2:
                line = line[1 : -2].split(',')
                line = map(float, line)
                right_datas.append(line)
                cnt = 0


    return [left_datas, right_datas]



dataset_names = ["Aloe", "Baby1", "Baby2", "Baby3", "Bowling1", "Bowling2", "Cloth1", "Cloth2", "Cloth3", "Cloth4", "Flowerpots", "Lampshade1", "Lampshade2", "Midd1", "Midd2", "Monopoly", "Plastic", "Rocks1", "Rocks2", "Wood1", "Wood2"]

def output(datas):

    len1 = len(datas)
    len2 = len(datas[0][0])
    print(len1, len2)


    names = ["spatial prop", "spatial prop + refine ", "spatial prop + refine + view prop", "spatial prop + refine + view prop + post-process", "spatial prop + post-process"]
    for i in range(len2):
        for j in range(len1):
            if j == 0:
                print("<tr>")
                print('<td>' + names[j] + '</td>')
                print('<td rowspan="5" style="text-align:center;vertical-align:middle;">' + dataset_names[i] + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][0]) + ', ' + str(datas[j][1][i][0]) + ')' + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][1]) + ', ' + str(datas[j][1][i][1]) + ')' + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][2]) + ', ' + str(datas[j][1][i][2]) + ')' + '</td>')
                print("</tr>")
            else:
                print("<tr>")
                print('<td>' + names[j] + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][0]) + ', ' + str(datas[j][1][i][0]) + ')' + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][1]) + ', ' + str(datas[j][1][i][1]) + ')' + '</td>')
                print('<td style="text-align:center;vertical-align:middle;">' + '(' + str(datas[j][0][i][2]) + ', ' + str(datas[j][1][i][2]) + ')' + '</td>')
                # print('<td>' + str(datas[j][0][i][0]) + '</td>')
                # print('<td>' + str(datas[j][0][i][1]) + '</td>')
                # print('<td>' + str(datas[j][0][i][2]) + '</td>')
                print("</tr>")



def output2(datas):

    len1 = len(datas)
    len2 = len(datas[0][0])
    print(len1, len2)


    names = ["spatial prop", "spatial prop + refine ", "spatial prop + refine + view prop", "spatial prop + refine + view prop + post-process", "spatial prop + post-process"]
    for i in range(len2):
        for j in range(len1):
            if j == 0:
                print(names[j]  + " & " + "\\multirow{5}{*}{" + dataset_names[i] + "} & "+ str(datas[j][0][i][0])+ " & " + str(datas[j][0][i][1]) + " & " + str(datas[j][0][i][2]) + "\\\\")
            else:
                print(names[j]  + " & " + " & " + str(datas[j][0][i][0])+ " & " + str(datas[j][0][i][1]) + " & " + str(datas[j][0][i][2]) + "\\\\")

    print('\\hlines')

if __name__ == '__main__':
    datas = []
    datas.append(read_data_from_file('log.txt'))
    datas.append(read_data_from_file('log2.txt'))
    datas.append(read_data_from_file('log3.txt'))
    datas.append(read_data_from_file('log4.txt'))
    datas.append(read_data_from_file('log5.txt'))
    output(datas)
    # output2(datas)





