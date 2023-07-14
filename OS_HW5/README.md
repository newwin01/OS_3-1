## ITP 30002-{02} Operating Systems, Spring 2023
### Homework 5. JsonFS using FUSE

### *Contributer: Sechang Jang*

---

Historically, Linux file systems were built directly into the kernel, necessitating modifications to the operating system's source code and extensive knowledge of kernel programming. FUSE (Filesystem in Userspace) streamlines the development procedure by offering a framework and a collection of APIs that enable file systems to be created in user space.

By utilizing FUSE, developers have the ability to craft custom file systems that can be mounted and interacted with just like any other file system on the system. These file systems can make use of diverse data sources or storage mechanisms, such as remote servers, databases, virtual files, or any other source accessible from user space.

You can see the presentation of this program in the following link: [Presentation Video](https://drive.google.com/file/d/1xZqj5LCA3bCbg2JJTe3Zuo9WsfCdYg4D/view?usp=sharing)

You need to use *build.sh* file to create binary file. Then, excute *run.sh*. By default, it creates directory **mnt** and use it as space for *FUSE*. 

If you want unmount *FUSE* you can execute *unmount.sh*. 

If you want to change json file that is read, you can change the bold section. 

*run.sh*
> *./fuse_example **./test2.json** ./mnt <br>*

If you want to change the directory that is used, you can change the bold sections. 

*run.sh*
> *./fuse_example ./test2.json **./mnt** <br>*

*unmout.sh*
> fusermount -u **./mnt**

<br>
<br>