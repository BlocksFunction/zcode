

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
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
    if (IsHave)
      *IsHave = (FilePtr != nullptr);
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

std::unordered_map<std::string, std::string> VarList;
bool FunctionFlags;
struct Function {
  std::string name;
  std::vector<std::string> code;
};

void CommandAST(std::string command) {
  if (command.front() == '#')
    return;
  else if (command.find("Output") != std::string::npos) {
    command.erase(0, command.find("Output") + 6);
    command.erase(std::ranges::remove(command, '\n').begin(), command.end());
    size_t pos = 0;
    while ((pos = command.find('{', pos)) != std::string::npos) {
      size_t endPos = command.find('}', pos);
      if (endPos == std::string::npos)
        break; // 未找到匹配的 }
      std::string varName = command.substr(pos + 1, endPos - pos - 1);
      if (VarList.contains(varName)) {
        command.replace(pos, endPos - pos + 1, VarList[varName]);
        pos += VarList[varName].length(); // 移动到替换后的位置
      } else {
        pos = endPos + 1; // 移动到下一个位置
      }
    }
    pos = 0;
    while ((pos = command.find("\\n", pos)) != std::string::npos) {
      command.replace(pos, 2, "\n");
      pos += 1;
    }

    if (auto size = command.find_last_of(',');
        size != std::string::npos && size > command.find_last_of('"')) {
      int speed = std::stoi(command.substr(size + 1));
      command.erase(std::ranges::remove(command, '"').begin(), command.end());
      command = command.substr(1, command.find_last_of(',') - 1);
      IO::printfs(command, speed);
    } else {
      command.erase(std::ranges::remove(command, '"').begin(), command.end());
      command = command.substr(1, command.length());
      printf("%s", command.c_str());
    }
  } else if (command.find("Input") != std::string::npos) {
    command.erase(0, command.find("Input") + 6);
    command.erase(std::ranges::remove(command, '\n').begin(), command.end());
    std::string temp;
    IO::scanfs(temp);
    if (VarList.contains(command)) {
      VarList[command] = temp;
    }
  } else if (command.find("exit") != std::string::npos)
    exit(EXIT_SUCCESS);
  else if (command.find('=') != std::string::npos) {
    size_t pos = command.find('=');
    std::string VarName = command.substr(0, pos);
    std::string VarValue = command.substr(pos + 1);
    // 去除空格
    VarName.erase(std::ranges::remove(VarName, ' ').begin(), VarName.end());
    VarValue.erase(std::ranges::remove(VarValue, ' ').begin(), VarValue.end());
    VarValue.erase(std::ranges::remove(VarValue, '\n').begin(), VarValue.end());
    VarList[VarName] = VarValue;
  }
}

void OnFile(const std::string &FilePath) {
  bool IsHave;
  const File file(FilePath, &IsHave);
  if (!IsHave)
    throw std::runtime_error("不存在文件!");
  for (const std::string &line : file.Read())
    CommandAST(line);
  exit(EXIT_SUCCESS);
}

[[noreturn]] void OnCommand() {
  printf("退出输入exit, 运行输入run\n");
  std::string command;
  std::vector<std::string> line;
  while (true) {
    command.clear();
    IO::scanfA(command, '\n');
    command.erase(std::ranges::remove(command, '\n').begin(), command.end());
    if (command == "run") {
      for (auto i : line)
        CommandAST(i);
      line.clear();
      printf("\n运行完成\n");
    } else {
      if (command == "exit")
        exit(EXIT_SUCCESS);
      line.push_back(command);
    }
  }
}

int main(int argv, char **argc) {
  std::printf("欢迎使用ZCode的解释器\n");
  if (argv > 1)
    try {
      OnFile(argc[1]);
    } catch (std::runtime_error &error) {
      printf("错误: %s", error.what());
    }
  else
    OnCommand();
  return 0;
}