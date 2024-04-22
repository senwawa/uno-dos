#include <SD.h>

bool doesNotStartWith(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) != 0;
}

bool doesNotEndWith(const char* str, const char* ending) {
    int str_len = strlen(str);
    int ending_len = strlen(ending);

    // If the string is shorter than the ending, it can't end with it
    if (str_len < ending_len) {
        return true;
    }

    // Compare the ending of the string with the specified ending
    return strncmp(str + str_len - ending_len, ending, ending_len) != 0;
}

bool startsWith(const char* str, const char* prefix) {
    // Use strncmp to compare the first 'n' characters of 'str' with 'prefix'
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

void replaceSubstring(char* str, const char* oldSubstr, const char* newSubstr) {
    // Find the position of the old substring
    char* pos = strstr(str, oldSubstr);
    
    // If the old substring is found
    if (pos != NULL) {
        // Calculate the lengths of the old and new substrings
        int oldSubstrLen = strlen(oldSubstr);
        int newSubstrLen = strlen(newSubstr);
        
        // Calculate the length difference
        int lengthDiff = newSubstrLen - oldSubstrLen;
        
        // Move the characters after the old substring to accommodate the new substring
        memmove(pos + newSubstrLen, pos + oldSubstrLen, strlen(pos + oldSubstrLen) + 1);
        
        // Copy the new substring into the position of the old substring
        memcpy(pos, newSubstr, newSubstrLen);
    }
}

char* mergeStrings(const char* str1, const char* str2) {
    char* result = new char[strlen(str1) + strlen(str2) + 1];
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

const int chipSelect = 10;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("uno-dos v0.0.1-ALPHA - SenMods");
  Serial.println("Waiting for Serial Input");
  Serial.println("");
};

void loop() {
  static char message[64]; // Buffer to store the incoming message
  static int messageIndex = 0; // Index to keep track of where to store the next byte
  
  // Check if there is serial data available
  if (Serial.available() > 0) {
    // Read the incoming byte
    char incomingByte = Serial.read();
    
    // Check if the received byte is the newline character ('\n')
    if (incomingByte == '\n') {
      // Null-terminate the message string
      message[messageIndex] = '\0';
      
      // Process the function
      processFunction(message);
      
      
      // Reset the message index to prepare for the next message
      messageIndex = 0;
    } else {
      // Store the received byte in the message buffer
      if (messageIndex < sizeof(message) - 1) {
        message[messageIndex++] = incomingByte;
      } else {
        // Buffer overflow, discard the rest of the message
        messageIndex = 0;
      };
    }
  }
};

void processFunction(const char* message) {
  // print
  if (strncmp(message, "print ", 6) == 0) {
    // Remove "print " from the message
    message += 6;
    print(message);
  } 
  
  // init_storage
  else if (strncmp(message, "init_storage", 12) == 0) {
    init_storage();
    message = message + 11;
  } 
  
  // ls
  else if (strncmp(message, "ls", 2) == 0) {
    const char* directory = message + 2;
    if (doesNotStartWith(directory, "/")) {
      directory = mergeStrings("/", directory);
    } else {
      directory = directory;
    }
    if (startsWith(directory, "/ ")) {
      replaceSubstring(directory, "/ ", "/");
    }
    if (startsWith(directory, "//")) {
      replaceSubstring(directory, "//", "/");
    }
    ls(directory);
  } 
  

  // mkdir
  else if (strncmp(message, "mkdir", 5) == 0) {
    const char* directory = message + 5;
    if (doesNotStartWith(directory, "/")) {
      directory = mergeStrings("/", directory);
    } else {
      directory = directory;
    }
    if (startsWith(directory, "/ ")) {
      replaceSubstring(directory, "/ ", "/");
    }
    if (startsWith(directory, "//")) {
      replaceSubstring(directory, "//", "/");
    }
    mkdir(directory);
  } 
  
  // command not found
  else {
    Serial.print("'");
    Serial.print(message);
    Serial.print("'");
    Serial.println(" is not a command/program.");
    Serial.println("");
  }
}


// Functions
void print(const char* message) {
  Serial.println(message);
  Serial.println("");
};

void init_storage() {
  Serial.println("Initializing SD Card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    Serial.println("");
    while (true); // Stop execution if initialization fails
  };
  Serial.println("SD card initialized successfully!");
  Serial.println("");
};

void ls(const char* dirName) {
  File root = SD.open(dirName);

  if (root) {
    Serial.println("Files in directory: " + String(dirName) + ":");
    Serial.println("");
    while (true) {
      File entry = root.openNextFile();
      if (!entry) {
        break; // No more files
      }
      Serial.println(entry.name());
      entry.close();
    }
    root.close();
    Serial.println("");
  } else {
    Serial.println("Error opening directory: " + String(dirName));
    Serial.println("");
  }
}

void mkdir(const char* folderName) {
  if (SD.mkdir(folderName)) {
    Serial.println("Folder created successfully: " + String(folderName));
    Serial.println("");
  } else {
    Serial.println("Error creating folder:" + String(folderName));
    Serial.println("");
  }
}
