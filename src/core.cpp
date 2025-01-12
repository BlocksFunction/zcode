#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace IO {
void scanfs(std::string &str) {
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != '\n' && tmp != ' ' &&
         tmp != EOF)
    str += tmp;
}

void scanfA(std::string &str, const char stopChar) {
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != stopChar)
    str += tmp;
}

void printfs(const std::string &str, double speed) {
  for (char c : str) {
    putchar(c);
    std::fflush(stdout);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(speed)));
  }
}
} // namespace IO

class File {
public:
  explicit File(const std::string &FileNames, bool *IsNew = nullptr)
      : FileName(FileNames), FilePtr(fopen(FileNames.c_str(), "r+")) {
    if (IsNew)
      *IsNew = FilePtr == nullptr;
  }

  ~File() {
    if (FilePtr)
      fclose(FilePtr);
  }

  bool Write(const std::string &str) const {
    return FilePtr && !str.empty() && fprintf(FilePtr, "%s", str.c_str()) > 0;
  }

  std::vector<std::string> Read() const {
    char line[1000];
    std::vector<std::string> arr;
    rewind(FilePtr);
    while (std::fgets(line, sizeof(line), FilePtr))
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
  bool IsNew;
  File file(filepath, &IsNew);
  if (IsNew) {
    printf("错误: %s 是一个不存在的文件\n", filepath.c_str());
  } else {
    for (std::string line : file.Read()) {
      if (line.find("Input") != std::string::npos) {
        line.erase(0, line.find("Input ") + 6);
        std::string text;
        int speed = 0;
        for (char c : line) {
          if (isalpha(c) || c == ' ')
            text += c;
          else if (isdigit(c))
            speed = speed * 10 + (c - '0');
        }
        if (speed)
          IO::printfs(text, speed);
        else
          printf("%s", text.c_str());
      }
    }
  }
}