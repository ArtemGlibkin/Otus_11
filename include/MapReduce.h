#pragma once
#include <fstream>
#include <filesystem>
#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <thread>
#include <atomic>

using MailContainer = std::list<std::string>;
void shuffle(MailContainer  & result, MailContainer  && in)
{
    result.merge(in);
}

MailContainer map(int mnum, const std::string & file, std::function<void(const std::string &, size_t, size_t, MailContainer &)> func)
{
    size_t file_size = std::filesystem::file_size(file);///home/sansforensics/Documents/Otus11/test.txt"
    size_t block_size = file_size / mnum;
    std::vector<std::unique_ptr<std::thread>> threads(mnum);
    
    int begin = 0;
    int end = begin + block_size;
    std::vector<MailContainer> result(mnum);
    MailContainer result_final;


    for(int i = 0; i < mnum; i++)
    {
        threads[i] = std::make_unique<std::thread>(func, file, begin, end, std::ref(result.at(i)));
        begin = end;
        if(i == mnum - 2)
            end = file_size;
        else
            end += block_size;
    }

    for(int i = 0; i < mnum; i++)
    {
        threads[i]->join();
        shuffle(result_final, std::move(result.at(i)));
    }
    return result_final;
}

void map_func(const std::string& file, size_t seek_begin, size_t seek_end, MailContainer& result)
{  
    std::ifstream fstr(file);
    if(seek_begin != 0)
    {
        fstr.seekg(seek_begin);
        std::string fake;
        std::getline(fstr, fake);
    }

    std::string tmp;
    while(fstr.tellg() < seek_end)
    {
        std::getline(fstr,tmp);
        result.push_back(tmp);
    }
    result.sort();
}

int strdiff(const std::string & str1, const std::string &str2)
{
    for(int i = 0; (i < str1.size() &&  i < str2.size()); i++)
        if(str1[i] != str2[i])
            return i;
}

void reduce(int runm, MailContainer & list_in, std::function<void(const MailContainer &)> func)
{
    int segment_size  = list_in.size() / runm;
    auto begin = list_in.begin();

    std::vector<std::pair<decltype(list_in.begin()), decltype(list_in.end())>> sectors(runm);
    std::vector<std::unique_ptr<std::thread>> threads(runm);
    
    for(int i = 0; i < runm; i++)
    {
        auto segm_end = (i == runm - 1) ? list_in.end() : std::next(begin, segment_size);
        
        for(auto it = segm_end; it != list_in.end(); it++)
        {
            if(it->at(0) != segm_end->at(0))
            {
                segm_end = it;
                break;
            }
        }
        sectors[i] = { begin, segm_end};
        begin = segm_end;
    }
    for(int i = 0; i < runm; i++)
        threads[i] = std::make_unique<std::thread>(func, MailContainer (sectors.at(i).first, sectors.at(i).second));
    for(int i = 0; i < runm; i++)
        threads[i]->join();
}

void reduce_function(const MailContainer  & list)
{
    static std::atomic<int> counter(1);
    int thread_id = counter++;

    int diffpred = 0;
    std::string file_name = std::to_string(thread_id) + ".txt";
    std::ofstream file(file_name);
    for(auto it = list.begin(), it2 = std::next(it); it != list.end() && it2 != list.end(); it++, it2++)
    {
        const std::string & str1 = *it;
        const std::string & str2 = *it2;
        int diff = strdiff(str1, str2);
        if(diff > diffpred) diffpred = diff;
        file<<str1.substr(0, diffpred + 1)<<std::endl;
        diffpred = diff;
    }
}