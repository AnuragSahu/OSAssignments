import subprocess
import random
import time

cmd_threaded = "./q3_threaded"
cmd_normal = "./q3_normal"
cmd_forked = "./q3_forked"

link ="-lpthread"

length = 100
number_of_samples = 350

filedescribtor = open("analysisMergeSort1.csv","w+")
filedescribtor.write(str("Length,Threaded,Normal,Forked\n"));


for a in range(number_of_samples):
    output = []
    output.append(str(length))
    for i in range(length):
        output.append(str(random.randint(1,12345)))
    print ("For threaded Mergresort.\n");
    #subprocess.call(["gcc",cmd_threaded,link]);
    output.insert(0,cmd_threaded);
    start_time = time.time();
    subprocess.call(output);
    end_time = time.time();
    time_threaded = end_time - start_time;
    print (output);
    output.pop(0);
    

    print ("For normal Mergresort.\n");
    #subprocess.call(["gcc",cmd_normal,link]);
    output.insert(0,cmd_normal);
    start_time = time.time();
    subprocess.call(output);
    end_time = time.time();
    time_normal = end_time - start_time;
    print (output);
    output.pop(0);

    print ("For forked Mergresort.\n");
    #subprocess.call(["gcc",cmd_forked,link]);
    output.insert(0,cmd_forked);
    start_time = time.time();
    subprocess.call(output);
    end_time = time.time();
    time_forked = end_time - start_time;
    print (output);
    output.pop(0);

    string_to_print = str(length)+","+str(time_threaded)+","+str(time_normal)+","+str(time_forked)+"\n";

    print(string_to_print);
    filedescribtor.write(string_to_print)
    if(length < 2000):
        length = length + 100;
    else:
        length = length + 500;