#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>


std::vector<unsigned char> loadFileIntoVector(std::fstream& fs) {
	unsigned char temp = 0;
	std::vector<unsigned char> fileContent;
	while (!fs.eof()) {
		fs >> temp;
		fileContent.push_back(temp);
	}
	return fileContent;
}

std::vector<unsigned char> filterHex(std::vector<unsigned char>& text) {
	const std::set<unsigned char> hexDigits = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	std::vector<unsigned char> filtredText;
	for (auto it = text.begin(); it != text.end(); ++it) {
		auto search = hexDigits.find(*it);
		if (search != hexDigits.end()) {
			filtredText.push_back(*it);
		}
	}
	return filtredText;

}

std::vector<unsigned char> compressChars(std::vector<unsigned char>& hexDigits) {
	size_t numberOfDigits = hexDigits.size();
	if (numberOfDigits & 1) {
		hexDigits.push_back('0');
		++numberOfDigits;
	}
	std::vector<unsigned char> compresedChars(numberOfDigits / 2);
	auto nextDigitIterator = hexDigits.begin();
	const std::map<unsigned char, unsigned char> hexCharToHexValue = {
		{ '0', 0 },{ '1', 1 },{ '2', 2 },{ '3', 3 },
		{ '4', 4 },{ '5', 5 },{ '6', 6 },{ '7', 7 },
		{ '8', 8 },{ '9', 9 },{ 'A', 10 },{ 'B', 11 },
		{ 'C', 12 },{ 'D', 13 },{ 'E', 14 },{ 'F', 15 }
	};
	for (size_t i = 0; i < compresedChars.size(); ++i) {
		unsigned char highHalf = hexCharToHexValue.at(*nextDigitIterator);
		++nextDigitIterator;
		unsigned char lowHalf = hexCharToHexValue.at(*nextDigitIterator);
		++nextDigitIterator;
		unsigned char fullByte = lowHalf | (highHalf << 4);
		compresedChars[i] = fullByte;
	}
	return compresedChars;
}

bool isOnesEven(unsigned char byte) {
	int onesNumber = 0;
	for (int i = 0; i < 8; ++i) {
		if ((1 << i) & byte) {
			++onesNumber;
		}
	}
	return ((onesNumber & 1) == 0);
}

int main(int argc, char*argv[]) {
	if (argc < 2) {
		std::cout << "Need file name as argument!\n";
		return -1;
	}

	std::fstream inFile;
	inFile.open(argv[1], std::ios::in);
	if (inFile.fail()) {
		std::cout << "Can`t open given file!\n";
		return -2;
	}
	std::vector<unsigned char> fileContent = loadFileIntoVector(inFile);
	inFile.close();
	std::vector<unsigned char> filtredText = filterHex(fileContent);
	std::vector<unsigned char> bytes = compressChars(filtredText);
	std::sort(bytes.begin(), bytes.end());

	std::fstream outFile;
	outFile.open("outFile.txt", std::ios::out);
	if (outFile.fail()) {
		std::cout << "Can`t create outFile.txt file!\n";
		return -3;
	}

	outFile << "EVEN\n";
	for (auto it = bytes.begin(); it != bytes.end(); ++it) {
		if (isOnesEven(*it)) {
			outFile << std::hex << static_cast<int>(*it) << " ";
		}
	}

	outFile << "\nODD\n";
	for (auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
		if (!isOnesEven(*it)) {
			outFile << std::hex << static_cast<int>(*it) << " ";
		}
	}

	outFile.close();

	return 0;
}