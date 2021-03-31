import os
import sys
methodNumber = sys.argv[1]
if methodNumber == '1':
    parametersArray = [700]
elif methodNumber == '2':
    parametersArray = [[200, 100]]
elif methodNumber == '3':
    parametersArray = [1, 2, 3, 4, 5, 6]
elif methodNumber == '4':
    parametersArray = [1, 2, 3, 5, 7, 9]
elif methodNumber == 'ec':
    parametersArray = []
paramDir = '../analysis/outputs/method_'+methodNumber
os.system('mkdir '+paramDir)
if (methodNumber != '4' and methodNumber != '3'):
    os.system('make obuild METHOD='+methodNumber)
else:
    os.system('make pbuild METHOD='+methodNumber)
for i in parametersArray:
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
    os.system('cd')
    os.system('ls')
    a=input()
    os.system('mv ../analysis/outputs/dynamic.out "'+workDir+"\"")
    os.system('mv ../analysis/outputs/static.out "'+workDir+"\"")
    os.system('mv ../analysis/outputs/time.out "'+workDir+"\"")
    os.system('mv ../analysis/outputs/timetaken.out "'+workDir+"\"")
    os.system('mv ../analysis/outputs/Plot.png "'+workDir+"\"")
