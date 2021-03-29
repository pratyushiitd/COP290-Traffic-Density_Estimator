import os
import sys
methodNumber = sys.argv[1]
if methodNumber == '1':
    a = [50, 40, 30, 20, 15, 10, 9, 8, 7, 6, 5, 4]
elif methodNumber == '2':
    a = [[200, 100], [200, 250], [3000, 300]]
elif methodNumber == '3':
    a = []
elif methodNumber == '4':
    a = [1, 2, 3, 5, 7, 9]
elif methodNumber == 'ec':
    a = []
paramDir = 'outputs/method_'+methodNumber
os.system('mkdir '+paramDir)
if methodNumber != '4':
    os.system('make obuild METHOD='+methodNumber)
else:
    os.system('make pbuild METHOD='+methodNumber)
for i in a:
    if(methodNumber == '2'):
        os.system('make orun METHOD='+methodNumber +
                  ' P1="'+str(i[0])+"\""+" P2="+str(i[1]))
    else:
        os.system('make orun METHOD='+methodNumber+' P1="'+str(i)+"\"")
    os.system('make plot')
    if methodNumber != '2':
        workDir = paramDir+"/"+str(i).strip()
    else:
        attach = ""
        if i[0] < 1000:
            attach += "0"+str(i[0])+"x"
        else:
            attach += str(i[0])+"x"
        if i[1] < 1000:
            attach += "0"+str(i[1])
        else:
            attach += str(i[0])
        workDir = paramDir+"/"+attach
    os.system('mkdir "'+workDir+"\"")
    os.system('mv outputs/dynamic.out "'+workDir+"\"")
    os.system('mv outputs/static.out "'+workDir+"\"")
    os.system('mv outputs/time.out "'+workDir+"\"")
    os.system('mv outputs/timetaken.out "'+workDir+"\"")
    os.system('mv outputs/Plot.png "'+workDir+"\"")
