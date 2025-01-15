#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/types.h>
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
  explicit File(const std::string &FileNames, bool *IsHave = nullptr)
      : FileName(FileNames), FilePtr(fopen(FileNames.c_str(), "r+")) {
    IsHave ? *IsHave = (FilePtr == nullptr) : NULL;
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

std::map<std::string, std::string> VarList;

void CommandAST(std::string command) {
  if (auto pos = command.find(',');
      pos != std::string::npos && command.find('"') == std::string::npos) {
    std::string VarName = command.substr(0, pos);
    std::string VarValue = command.substr(pos + 1);
    // 去除空格
    VarName.erase(std::ranges::remove(VarName, '"').begin(), VarName.end());
    VarValue.erase(std::ranges::remove(VarName, '"').begin(), VarName.end());
    VarValue.erase(std::ranges::remove(VarName, '\n').begin(), VarName.end());
    VarList[VarName] = VarValue;
  } else if (command.find("Output") != std::string::npos) {
    command.erase(0, command.find("Output") + 6);
    std::string text =
        command.substr(command.find('"'), command.find_last_of(','));
    for (size_t size = 0; (size = text.find('{', size)) != std::string::npos;) {
      const size_t endPos = text.find('}', size);
      if (endPos == std::string::npos)
        break;
      std::string VarName = text.substr(size + 1, endPos - size - 1);
      if (VarList.contains(VarName)) {
        text.replace(size, endPos - size + 1, VarList[VarName]);
        size += VarList[VarName].length();
      } else
        size = endPos + 1;
    }
    std::string OutText;
    for (size_t i = 0; i < text.length(); ++i)
      if (text[i] == '\\' && i + 1 < text.length() && text[i + 1] == 'n')
        OutText += '\n';
    if (auto pos = command.find_last_of(',');
        pos != std::string::npos && pos > command.find_last_of('"')) {
      int speed = std::stoi(command.substr(pos + 1));
      command.erase(std::ranges::remove(command, '"').begin(), command.end());
      command = command.substr(1, command.find_last_of(',') - 1);
      IO::printfs(command, speed);
    } else {
      command.erase(std::ranges::remove(command, '"').begin(), command.end());
      command = command.substr(1, command.length());
      printf("%s", command.c_str());
    }
  } else if (command.find("exit") != std::string::npos)
    exit(EXIT_SUCCESS);
}

void OnFile(std::string FilePath) {
  bool IsHave;
  File file(FilePath, &IsHave);
  if (IsHave)
    throw std::runtime_error("不存在文件!");
  for (std::string line : file.Read())
    CommandAST(line);
}

void OnCommand() {
  std::string command;
  while (true) {
    command.clear();
    IO::scanfA(command, '\n');
    CommandAST(command);
    putchar('\n');
  }
}

int main(int argv, char **argc) {
  std::printf("欢迎使用ZCode的解释器\n");
  if (argv > 1)
    try {
      OnFile(argc[0]);
    } catch (std::runtime_error &error) {
      printf("错误: %s", error.what());
    }
  else
    OnCommand();
  return 0;
  // if () {}
}