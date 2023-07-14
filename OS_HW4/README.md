## ITP 30002-{02} Operating Systems, Spring 2023
### Homework 4. findeq: multithreaded search of files with equal data

### *Contributer: Sechang Jang, Hayoung Jang, Junehee Cho*

---

The File Stroage Capacity have been increased with the development of hardware and software. Starting from using the Punch Card or Punch Tape, Magenetic Tape have been used (using nowadays, too). Moving from the Floppy Disk and Compact Disk, Hard Dist Drive is using widely. Nowadays, Solid State Drive is used popularly with the combination of Large Size Hard Disk Drive. More than that, Cloud Storage is becoming trending storage device.

Within the large capacity of storage device, people treat duplicate files without any care. It might be not any problem with personal Hard Disk Drive or Solid State Drive. However, in the Cloud Storage System, the capacity is equal to money. As much you occupies space, you might need to pay more money. Therefore, finding equivalent file is required in the Cloud Storage.

File comparing work is simple but takes time. Therefore, *findep* works as multithreading program. 

You can see the presentation of this program in the following link: [Presentation Video](https://drive.google.com/file/d/1u22T0rhMDrcfXfjZNV6yO7og09QMnHGp/view?usp=share_link)

It works with following options

> *t: number of threads (give number is no more than 64, greater than 1) | m: ignores size is less than certain bytes | o: file to store the result (optional) <br>*

> After that, provide directory path (./dir)

It has two version of the findep, with single producer and multiproducer. We can see the difference of using multiple producer and single producer

> *make* <br>
./single -m 1000 -t 10 ./dir <br>
./multi -m 1000 -t 10 ./dir

To measure the time sequence of each program, *make timer* give an option to measure the time. *test.sh* helps to perform experiment. 

> *make timer* <br>
./single_timer -m 1000 -t 10 ./dir <br>
./multi_timer -m 1000 -t 10 ./dir <br>
...

<br>
<br>

##### To see overall execution of algorithm, you can use -D_DEBUG option when you compile the code.