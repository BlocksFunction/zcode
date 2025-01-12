#include <cstdio>
#include <regex>
#include <string>
#include <vector>

namespace IO {
// 字符串输入
void scanfs(std::string &str) {
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != '\n' && tmp != ' ' &&
         tmp != EOF)
    str += tmp;
}

// 单字符停止
void scanfA(std::string &str, const char stopChar) {
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != stopChar)
    str += tmp;
}
} // namespace IO

class File {
public:
  explicit File(const std::string &FileNames, bool *IsNew = nullptr) {
    if (IsNew) {
      if (fopen(FileNames.c_str(), "r") == nullptr) {
        *IsNew = true;
      } else {
        *IsNew = false;
      }
    }
    this->FilePtr = fopen(FileNames.c_str(), "r+");
    this->FileName = FileNames;
  }

  ~File() {
    if (FilePtr) {
      fclose(FilePtr);
    }
  }

  [[nodiscard]] bool Write(std::string &str) const {
    if (FilePtr) {
      if (!str.empty() && str[0] == '\n')
        str.erase(0, str.find_first_not_of('\n'));
      fprintf(this->FilePtr, "%s", str.c_str());
    } else {
      return false;
    }
    return true;
  }

  [[nodiscard]] FILE *GetFilePtr() const { return FilePtr; }

  [[nodiscard]] std::vector<std::string> Read() const {
    char line[1000];
    std::vector<std::string> arr;
    rewind(this->FilePtr);
    while (std::fgets(line, sizeof(line), this->FilePtr) != nullptr)
      arr.push_back(line);
    return arr;
  }

private:
  std::string FileName;
  FILE *FilePtr;
};

int main() {
  printf("ZCode: 解释器\n文件地址: ");
  std::string filepath;
  IO::scanfA(filepath, '\n');
  bool IsNew = true;
  File file(filepath, &IsNew);
  if (IsNew)
    std::printf("错误: %s 是一个不存在的文件\n", filepath.c_str());
  else {
    std::vector<std::string> a = file.Read();
    for (std::string i : a) {
      if (i.find("Input") != std::string::npos) {
        std::regex pattern("Input");
        i = std::regex_replace(i, pattern, "");
        printf("%s", i.c_str());
      }
    }
  }
}