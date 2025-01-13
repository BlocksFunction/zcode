#include <algorithm>
#include <chrono>
#include <cstdio>
#include <map>
#include <ranges>
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
    if (c != '\n') { // 只有在字符不是换行符时才引入延迟
      std::fflush(stdout);
      std::this_thread::sleep_for(
          std::chrono::milliseconds(static_cast<int>(speed)));
    }
  }
}
} // namespace IO

class File {
public:
  explicit File(const std::string &FileNames, bool *IsNew = nullptr)
      : FileName(FileNames), FilePtr(fopen(FileNames.c_str(), "r+")) {
    if (IsNew)
      *IsNew = (FilePtr == nullptr);
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
  bool IsNotHave;
  File file(filepath, &IsNotHave);
  if (IsNotHave) {
    printf("错误: %s 是一个不存在的文件\n", filepath.c_str());
  } else {
    std::map<std::string, std::string> varList;
    for (std::string line : file.Read()) {
      if (auto pos = line.find('='); pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        key.erase(std::ranges::remove(key, ' ').begin(), key.end());
        value.erase(std::ranges::remove(value, ' ').begin(), value.end());
        value.erase(std::ranges::remove(value, '\n').begin(), value.end());
        varList[key] = value;
      } else if (line.find("Input") != std::string::npos) {
        line.erase(0, line.find("Input ") + 6);
        auto [text, speed] = [&line] {
          size_t commaPos = line.find(',');
          return std::make_pair(line.substr(0, commaPos),
                                std::stoi(line.substr(commaPos + 1)));
        }();
        if (text.front() == '"' && text.back() == '"')
          text = text.substr(1, text.length() - 2);
        for (size_t size = 0;
             (size = text.find('{', size)) != std::string::npos;) {
          const size_t endPos = text.find('}', size);
          if (endPos == std::string::npos)
            break;
          std::string varName = text.substr(size + 1, endPos - size - 1);
          if (varList.contains(varName)) {
            text.replace(size, endPos - size + 1, varList[varName]);
            size += varList[varName].length();
          } else {
            size = endPos + 1;
          }
        }
        std::string processedText;
        for (size_t i = 0; i < text.length(); ++i) {
          if (text[i] == '\\' && i + 1 < text.length() && text[i + 1] == 'n') {
            processedText += '\n';
            i++;
          } else {
            processedText += text[i];
          }
        }
        if (speed)
          IO::printfs(processedText, speed);
        else
          printf("%s", processedText.c_str());
      }
    }
  }
}