#include <iostream>
#include <unistd.h>

int main(){
    //execl("/usr/bin/gnome-screenshot", "gnome-screenshot", "-f", "testp" , NULL);
    execl("/bin/cat", "/bin/cat", "temp", NULL);
    return 0;
}

