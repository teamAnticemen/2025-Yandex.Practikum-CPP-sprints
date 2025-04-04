#include <cassert> 
#include <filesystem> 
#include <fstream> 
#include <iostream> 
#include <regex> 
#include <sstream> 
#include <string> 
#include <vector> 
 
using namespace std; 
using filesystem::path; 
 
path operator""_p(const char* data, std::size_t sz) { 
    return path(data, data + sz); 
} 
 
bool recursive = false; 
// Функция для обработки и включения файлов 
bool ProcessIncludes(const path& current_file, const vector<path>& include_directories, ofstream& out_stream) { 
    ifstream in_stream(current_file); 
    if (!in_stream.is_open()) {  
        return false; // Не удалось открыть текущий файл  
    } 
    //cout << current_file.string() << " recursived times > "s << endl; 
    string line; 
    int line_number = 0; 
                            
    regex include_dquotes(R"(\s*#\s*include\s*"([^"]*)\"\s*)"); 
    regex include_angle  (R"(\s*#\s*include\s*<([^>]*)>\s*)"); 
     
   while (getline(in_stream, line)) { 
        line_number++; 
         
        smatch match; 
       // Проверяем на наличие директивы #include "..." 
        if (regex_match(line, match, include_dquotes)) { 
            path included_file = current_file.parent_path() / match[1].str(); // Путь относительно текущего файла 
            ifstream included_stream(included_file); 
             
            if (!included_stream.is_open()) { 
                // Если не удалось открыть, продолжаем искать в include_directories 
                for (const auto& dir : include_directories) { 
                    included_file = dir / match[1].str(); 
                    included_stream.open(included_file); 
                    if (included_stream.is_open()) { 
                        break; // Файл найден 
                    } 
                } 
            } 
             
            if (!included_stream.is_open()) { 
                cout << "unknown include file " << match[1].str()  
                     << " at file " << current_file.string()  
                     << " at line " << line_number << endl; 
                return false; // Уведомляем об ошибке и выходим 
            } 
             
            // Записываем содержимое включаемого файла 
            string included_line; 
            while (getline(included_stream, included_line)) { 
                ProcessIncludes(included_file, include_directories, out_stream); 
                break; 
                //out_stream << included_line << endl; // Пишем включаемый файл в выходной поток 
            } 
            included_stream.close(); 
            continue; // Переходим к следующей строке основного файла 
        } 
 
        // Проверяем на наличие директивы #include <...> 
        else if (regex_match(line, match, include_angle)) { 
            path included_file = current_file.parent_path() / match[1].str(); // Путь относительно текущего файла 
            ifstream included_stream(included_file); 
             
            if (!included_stream.is_open()) { 
                // Если не удалось открыть, продолжаем искать в include_directories 
                for (const auto& dir : include_directories) { 
                    included_file = dir / match[1].str(); 
                    included_stream.open(included_file); 
                    if (included_stream.is_open()) {                         
                        break; // Файл найден 
                    } 
                } 
            } 
             
            if (!included_stream.is_open()) { 
                cout << "unknown include file " << match[1].str()  
                     << " at file " << current_file.string()  
                     << " at line " << line_number << endl; 
                return false; // Уведомляем об ошибке и выходим 
            } 
              
            // Записываем содержимое включаемого файла 
            string included_line; 
            while (getline(included_stream, included_line)) { 
                ProcessIncludes(included_file, include_directories, out_stream); 
                 break; 
                //out_stream << included_line << endl; // Пишем включаемый файл в выходной поток 
            } 
            included_stream.close(); 
            continue; // Переходим к следующей строке основного файла 
        } 
         
       else{ 
        // Записываем строку, если это не директива #include 
        out_stream << line << endl; 
       } 
    } 
 
    in_stream.close(); 
    return true; // Все файлы успешно обработаны 
} 
 
// Реализуйте функцию Preprocess 
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) { 
    ofstream out_stream(out_file, ios::ate); 
    if (!out_stream.is_open()) { 
        return false; // Не удалось открыть файл для записи 
    } 
    return ProcessIncludes(in_file, include_directories, out_stream); 
} 
 
string GetFileContents(string file) { 
    ifstream stream(file); 
    // Конструируем string по двум итераторам 
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()}; 
} 
 
void Test() { 
    error_code err; 
    filesystem::remove_all("sources"_p, err); 
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err); 
    filesystem::create_directories("sources"_p / "include1"_p, err); 
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err); 
 
    { 
        ofstream file("sources/a.cpp"); 
        file << "// this comment before include\n" 
                "#include \"dir1/b.h\"\n" 
                "// text between b.h and c.h\n" 
                "#include \"dir1/d.h\"\n" 
                "\n" 
                "int SayHello() {\n" 
                "    cout << \"hello, world!\" << endl;\n" 
                "#   include<dummy.txt>\n" 
                "}\n"s; 
    } 
    { 
        ofstream file("sources/dir1/b.h"); 
        file << "// text from b.h before include\n" 
                "#include \"subdir/c.h\"\n" 
                "// text from b.h after include"s; 
    } 
    { 
        ofstream file("sources/dir1/subdir/c.h"); 
        file << "// text from c.h before include\n" 
                "#include <std1.h>\n" 
                "// text from c.h after include\n"s; 
    } 
    { 
        ofstream file("sources/dir1/d.h"); 
        file << "// text from d.h before include\n" 
                "#include \"lib/std2.h\"\n" 
                "// text from d.h after include\n"s; 
    } 
    { 
        ofstream file("sources/include1/std1.h"); 
        file << "// std1\n"s; 
    } 
    { 
        ofstream file("sources/include2/lib/std2.h"); 
        file << "// std2\n"s; 
    } 
 
    Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p, 
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p}); 
     
    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p, 
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p}))); 
 
    ostringstream test_out;     
    test_out << "// this comment before include\n" 
                "// text from b.h before include\n" 
                "// text from c.h before include\n" 
                "// std1\n" 
                "// text from c.h after include\n" 
                "// text from b.h after include\n" 
                "// text between b.h and c.h\n" 
                "// text from d.h before include\n" 
                "// std2\n" 
                "// text from d.h after include\n" 
                "\n" 
                "int SayHello() {\n" 
                "    cout << \"hello, world!\" << endl;\n"s; 
     
    //string contents = GetFileContents("sources/a.in"s);// Для отладки 
    // Распечатываем содержимое файла в cout 
    //cout << contents << " GetFileContents "s << endl; 
     
    assert(GetFileContents("sources/a.in"s) == test_out.str()); 
} 
 
int main() { 
    Test(); 
}