import os
a = [50, 40, 30, 20, 15, 10, 9, 8, 7, 6, 5, 4]
for i in a:
    os.system('make run PARAM='+str(i))
    os.system('make plot')
    os.system('cd outputs')
    os.system('mkdir outputs/'+str(i))
    os.system('mv outputs/dynamic.out outputs/'+str(i))
    os.system('mv outputs/static.out outputs/'+str(i))
    os.system('mv outputs/time.out outputs/'+str(i))
    os.system('mv outputs/timetaken.out outputs/'+str(i))
    os.system('mv outputs/Plot.png outputs/'+str(i))
