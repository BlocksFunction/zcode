#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
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

// 延迟输出
void printfs(const std::string &str, double speed) {
  if (!str.empty()) {
    for (char c : str) {
      putchar(c);
      std::fflush(stdout);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(speed)));
    }
  }
}
} // namespace IO

class File {
public:
  explicit File(const std::string &FileNames, bool *IsNew = nullptr) {
    if (IsNew) {
      *IsNew = fopen(FileNames.c_str(), "r") == nullptr;
    }
    this->FilePtr = fopen(FileNames.c_str(), "r+");
    this->FileName = FileNames;
  }

  ~File() {
    if (FilePtr) {
      fclose(FilePtr);
    }
  }

  [[nodiscard]] bool Write(const std::string &str) const {
    if (FilePtr && !str.empty()) {
      fprintf(this->FilePtr, "%s", str.c_str());
      return true;
    }
    return false;
  }

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
    printf("错误: %s 是一个不存在的文件\n", filepath.c_str());
  else {
    for (std::string line : file.Read()) {
      if (line.find("Input") != std::string::npos) {
        line.erase(0, line.find("Input ") + 6);
        std::string text;
        int speed = 0;
        for (char c : line) {
          if (isalpha(c) || c == ' ') {
            text += c;
          } else if (isdigit(c)) {
            speed = speed * 10 + (c - '0');
          }
        }
        if (speed)
          IO::printfs(text, speed);
        else
          printf("%s", text.c_str());
      }
    }
  }
}