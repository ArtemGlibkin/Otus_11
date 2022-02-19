
#include "MapReduce.h"
#include <list>
#include <string>

int main(int argc, char * argv[])
{
    if(argc != 4)
    {
        std::cout<<"mapreduce [src file] [mnum] [rnum]"<<std::endl;
        return -1;
    }
    int mnum = std::atoi(argv[2]);
    int rnum = std::atoi(argv[3]);

    std::list<std::string> result = map(mnum, argv[1], map_func);
    reduce(rnum, result, reduce_function);
    return 0;
}