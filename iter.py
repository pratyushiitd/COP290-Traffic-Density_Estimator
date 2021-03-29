import os
a = [1, 2, 3, 5, 7, 9]
for i in a:
    os.system('make run PARAM='+str(i))
    os.system('make plot')
    os.system('cd outputs')
    os.system('mkdir outputs/'+str(i))
    os.system('mv outputs/dynamic.out outputs/'+str(i))
    os.system('mv outputs/sparse.out outputs/'+str(i))
    os.system('mv outputs/time.out outputs/'+str(i))
    os.system('mv outputs/timetaken.out outputs/'+str(i))
    os.system('mv outputs/Plot.png outputs/'+str(i))
