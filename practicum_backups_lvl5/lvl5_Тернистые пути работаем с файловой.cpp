Sign in New API Help About
2.4 KB of C++
Created 5 days, 15 hours ago — expires in 25 days
Viewed 5 times
COPY TO CLIPBOARD SOFT WRAP RAW TEXT DUPLICATE DIFF
 1
 2
 3
 4
 5
 6
 7
 8
 9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <string_view>
#include <typeinfo>

using namespace std;
using namespace std::literals;
using filesystem::path;
namespace fs = std::filesystem;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

int depth = 0;
bool firstCheck = false;
// напишите эту функцию
void PrintTree(ostream& dst, const path& p) {
    // Получаем список элементов в директории
    std::vector<fs::directory_entry> entries;
    for (const auto& entry : fs::directory_iterator(p)) {
        entries.push_back(entry);
    }

    if(!firstCheck && entries.empty())
    {
        dst << p.filename().string() << endl;
        return;
    }
    firstCheck = true;
    
    // Сортируем элементы в обратном алфавитном порядке
    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        return a.path().filename().string() > b.path().filename().string();
    });

    // Выводим отступы для уровня глубины
    std::string indent(depth, ' ');    
    
     
    // Обходим и выводим элементы
    for (const auto& entry : entries) {
        if(depth == 0)
        {
            dst << indent << p.string() << '\n';
            depth += 2;
        }
        std::string indent(depth, ' ');
        dst << indent << entry.path().filename().string();
        
        // Если это папка, рекурсивно вызываем PrintTree
        if (fs::is_directory(entry)) {            
            dst << '\n'; 
            depth += 2;
            PrintTree(dst, entry.path());           
        }
        else dst << std::endl;
    }
    depth = 2;
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);//Закоментированно для теста пустой папки
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    //ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    //ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    //ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    cout << out.str() << endl;
    assert(out.str() ==
        "test_dir\n"
        //"  b\n"
        //"    f2.txt\n"
        //"    f1.txt\n"
        //"  a\n"
        //"    f3.txt\n"s
    );
}
Share:   
IP blocking middleware for Django sites — No web server config required. Usable on any PaaS.
Ads by EthicalAds
