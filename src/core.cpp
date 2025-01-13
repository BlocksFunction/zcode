#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace IO {
void scanfs(std::string &str) { // 输入字符串
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != '\n' && tmp != ' ' &&
         tmp != EOF)
    str += tmp;
}

void scanfA(std::string &str, const char stopChar) { // 输入字符串x2
  char tmp;
  while ((tmp = static_cast<char>(getchar())) != stopChar)
    str += tmp;
}

void printfs(const std::string &str, double speed) { // 输出
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

  [[nodiscard]] bool Write(const std::string &str) const {
    return FilePtr && !str.empty() && fprintf(FilePtr, "%s", str.c_str()) > 0;
  }

  [[nodiscard]] std::vector<std::string> Read() const {
    if (!FilePtr)
      return {};
    char line[1000];
    std::vector<std::string> arr;
    rewind(FilePtr);
    while (std::fgets(line, sizeof(line), FilePtr))
      arr.emplace_back(line);
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
    // 定义变量列表
    std::map<std::string, std::string> varList = {};

    for (std::string line : file.Read()) {
      if (line.find("=") != std::string::npos) {
        std::string right, left;
        const size_t lc = line.find('=');
        right = line.substr(0, lc);
        right.erase(std::remove(right.begin(), right.end(), ' '), right.end());
        left = line.substr(lc + 1, line.length());
        left.erase(std::remove(left.begin(), left.end(), ' '), left.end());
        varList[right] = left;
      }
      if (line.find("Input") != std::string::npos) {
        line.erase(0, line.find("Input ") + 6);
        if (size_t commaPos = line.find(','); commaPos != std::string::npos) {
          std::string text = line.substr(0, commaPos);
          int speed = std::stoi(line.substr(commaPos + 1));
          if (text.front() == '"' && text.back() == '"') {
            text = text.substr(1, text.length() - 2);
          }

          // 处理变量引用
          size_t pos = 0;
          while ((pos = text.find("{", pos)) != std::string::npos) {
            size_t endPos = text.find("}", pos);
            if (endPos == std::string::npos)
              break; // 未找到匹配的 }
            std::string varName = text.substr(pos + 1, endPos - pos - 1);
            if (varList.find(varName) != varList.end()) {
              text.replace(pos, endPos - pos + 1, varList[varName]);
            }
            pos += varList[varName].length(); // 移动到替换后的位置
          }

          // 处理转义字符
          std::string processedText;
          for (size_t i = 0; i < text.length(); ++i) {
            if (text[i] == '\\' && i + 1 < text.length()) {
              if (text[i + 1] == 'n') {
                processedText += '\n', i++;
              } else
                processedText += text[i];
            } else
              processedText += text[i];
          }

          if (speed)
            IO::printfs(processedText, speed);
          else
            printf("%s", processedText.c_str());
        }
      }
    }
  }
}