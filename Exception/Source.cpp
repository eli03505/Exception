#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime>

using namespace std;

class Date
{
	struct date
	{
		unsigned short second;
		unsigned short minute;
		unsigned short hour;
		unsigned short day;
		unsigned short month;
		unsigned short year;
	};

	date date;
public:
	void set()
	{
		time_t now(time(0));
		tm newtime;
		localtime_s(&newtime, &now);
		date.year = newtime.tm_year + 1900;
		date.month = newtime.tm_mon + 1;
		date.day = newtime.tm_mday;
		date.hour = newtime.tm_hour;
		date.minute = newtime.tm_min;
		date.second = newtime.tm_sec;
	}

	void show() const
	{
		cout << this->date.day << '/' << this->date.month << '/' << this->date.year << ' ';
		cout << this->date.hour << ':' << this->date.minute << ':' << this->date.second << endl;

	}
};

class Disc
{
private:
	class File
	{
	private:
		double file_size = 0;
		bool hashed;
	public:
		string file_name;
		string content;
		Date file_created;
		Date last_modified;

		double getFileSize() const { return this->file_size; }

		File(const string& filename, const string& content = "")
		{
			this->file_name = filename;
			this->content = content;
			this->file_size = content.length();
			this->hashed = false;
		}

		void setFileSize(size_t size)
		{
			this->file_size = size;
		}

		void setHashedStatus(bool status)
		{
			this->hashed = status;
		}

		bool getHashedStatus() const { return this->hashed; }

		void show() const
		{
			system("Color XY");
			cout << "File name: " << this->file_name << endl;
			cout << "File created: "; this->file_created.show();
			cout << "Last modified: "; this->last_modified.show();
			cout << "File size: " << this->file_size << endl;
			cout << "File hashed: " << boolalpha << this->getHashedStatus() << endl;
			cout << "File content: " << this->content << endl;
		}
	};

	string disk_name;
	string model;
	File** files;
	size_t count;
	double current_size;
	Date disk_created;
	Date last_modified;

	size_t generateHash(const string& content)
	{
		hash<string > hash;
		return hash(content);
	}
public:


	static double capacity;

	Disc(const string& disk_name, const string& model) :files(NULL), count(0), current_size(0) {
		setDiskName(disk_name);
		setModel(model);
		disk_created.set();
		last_modified.set();
	}

	void setDiskName(const string& disk_name)
	{
		this->disk_name = disk_name;
	}

	string getDiskName() const { return this->disk_name; }

	void setModel(const string& model)
	{
		this->model = model;
	}

	string getModel() const { return this->model; }

	void hashFile(const string& file_name)
	{
		if (!isExists(file_name))
			throw file_name + " isn't exist!";

		File* file = getFile(file_name);

		string content = to_string(generateHash(file->content));
		size_t new_size = content.length();

		size_t current_size = file->getFileSize();

		if (new_size > current_size)
			if (this->current_size + new_size - current_size >= capacity)
				throw string("HDD is full!");

		file->setFileSize(new_size);
		file->content = content;
		file->setHashedStatus(true);
		file->last_modified.set();
		this->last_modified.set();
		this->current_size = (this->current_size - current_size) + new_size;
		readWriteSpeed(new_size);
	}

	virtual void readWriteSpeed(size_t bytes) const = 0;

	File* getFile(const string& file_name) throw(string)
	{
		//eger file yoxdursa error atsin // ex : bele bir fayl movcud deyil
		for (size_t i = 0; i < this->count; i++)
		{
			if (file_name == files[i]->file_name)
			{
				return files[i];
			}
		}
		throw file_name + " doesn't exist!";
	}
	bool isExists(const string& file_name)
	{
		//eger varas true or false

		for (size_t i = 0; i < this->count; i++)
		{
			if (file_name == files[i]->file_name)
			{
				return true;
			}
		}

		return false;
	}

	void createFile(const string& file_name, const string& content = "") throw(string)
	{
		//1. eger fayl varsa error at, yeni fayli movcuddur
		//2. eger fayl yoxdursa ve hdd doludursa error at, disk doludur.

		if (isExists(file_name))
			throw file_name + " is exist!";
		else if (this->current_size + content.length() >= capacity)
			throw string("HDD is full!");

		size_t new_size = this->count + 1;

		auto new_files = new File * [new_size];

		if (new_files != NULL)
		{
			if (new_size - 1)
			{
				for (size_t i = 0; i < new_size - 1; i++)
				{
					new_files[i] = this->files[i];
				}

				delete[] this->files;
			}

			File* new_file = new File(file_name, content);
			new_file->file_created.set();
			new_file->last_modified.set();
			this->last_modified.set();
			if (new_file != NULL)
			{
				new_files[new_size - 1] = new_file;
			}
			else
				throw file_name + " didn't created!";

			this->count = new_size;
			this->files = new_files;
			this->current_size += new_file->getFileSize();

			readWriteSpeed(new_file->getFileSize());

			cout << "File created -> " << new_file->file_name << endl;
		}
		else {
			throw ("Files can't copied!");
		}
	}

	void setFileContent(const string& file_name, const string& content) throw(string)
	{
		//1. eger fayl yoxdursa error at
		//2. eger fayl varsa ve disk doludursa error.
		if (!isExists(file_name))
			throw file_name + " isn't exist!";

		File* file = getFile(file_name);

		size_t new_size = content.length();

		size_t current_size = file->getFileSize();

		if (new_size > current_size)
			if (this->current_size + new_size - current_size >= capacity)
				throw string("HDD is full!");

		file->setFileSize(new_size);
		file->content = content;
		file->last_modified.set();
		this->last_modified.set();

		this->current_size = (this->current_size - current_size) + new_size;
		readWriteSpeed(new_size);
	}

	void deleteFileContent(const string& file_name) throw(string)
	{
		//1. eger fayl yoxdursa error at
		if (!isExists(file_name))
			throw file_name + " isn't exist!";

		File* file = getFile(file_name);

		this->current_size -= file->getFileSize();
		file->setFileSize(0);

		file->content = "";
		file->last_modified.set();
		this->last_modified.set();
	}

	void deleteFileByFileName(const string& file_name)
	{
		if (!isExists(file_name)) {
			throw file_name + " isn't exist";
		}

		size_t new_size = this->count - 1;

		auto new_files = new File * [new_size];

		if (new_files != NULL)
		{
			size_t file_index = 0;
			for (size_t i = 0, j = 0; i < this->count; i++)
			{
				if (files[i]->file_name != file_name)
					new_files[j++] = files[i];
				else
					file_index = i;
			}

			delete files[file_index];
			delete[] files;

			files = new_files;
			this->count = new_size;
			this->last_modified.set();
		}
		else {
			throw "Files can't copied!";
		}
	}

	void updateFileName(const string& file_name, const string& new_file_name)
	{
		if (!isExists(file_name))
			throw file_name + " isn't exist";

		for (size_t i = 0; i < this->count; i++)
		{
			if (files[i]->file_name == file_name)
			{
				files[i]->file_name = new_file_name;
				files[i]->last_modified.set();
				this->last_modified.set();
				return;
			}
		}
	}

	void showAllFiles() const
	{
		for (size_t i = 0; i < this->count; i++)
		{
			cout << "######################################" << endl;
			this->files[i]->show();
			readWriteSpeed(this->files[i]->getFileSize());
		}
	}

	void showHDDInfo() const
	{
		system("Color B");
		cout << "Disk name: " << getDiskName() << endl;
		cout << "Model: " << getModel() << endl;
		cout << "Capacity: " << this->capacity << " bytes" << endl;
		cout << "Used: " << this->current_size << " bytes" << endl;
		cout << "Free: " << this->capacity - this->current_size << " bytes" << endl;
		cout << "Disk created: "; this->disk_created.show();
		cout << "Last modified: "; this->last_modified.show();
	}

	virtual ~Disc() = 0 {}
};

double Disc::capacity = 2097152; //bytes


class HDD : public Disc
{
public:

	HDD(const string& disk_name, const string& model) : Disc(disk_name, model) {}

	void readWriteSpeed(size_t bytes) const
	{
		Sleep(bytes * 10);
	}
};

class SSD : public Disc
{
public:
	SSD(const string& disk_name, const string& model) : Disc(disk_name, model) {}

	void readWriteSpeed(size_t bytes) const
	{
		Sleep(bytes);
	}
};


class Operation
{
public:
	Operation() = delete;

	static void controlPC();
};

void Operation::controlPC()
{
	HDD hdd("my_disk", "Acer");

	hdd.showHDDInfo();
	cout << "###########################" << endl;
	try
	{
		hdd.createFile("first_file.txt", "Hello world");
		hdd.createFile("account.txt", "username: test\npassword: test");
		hdd.createFile("notes.txt", "Some notes...");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	try
	{
		hdd.getFile("first_file.txt")->show();
		hdd.getFile("first_file2.txt")->show();
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.setFileContent("notes.txt", "Notes updated...");
		hdd.setFileContent("noted.txt", "new file");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.deleteFileByFileName("account.txt");
		hdd.deleteFileByFileName("account.txt");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.updateFileName("notes.txt", "important_notes.txt");
		hdd.updateFileName("notes.txt", "important_notes.txt");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.deleteFileContent("important_notes.txt");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.createFile("account.txt", "username: test\npassword: test1234");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}

	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	try
	{
		hdd.hashFile("account.txt");
	}
	catch (string ex)
	{
		cout << "Error: " << ex << endl;
	}
	cout << endl;

	hdd.showAllFiles();
	cout << endl;

	hdd.showHDDInfo();
	
};


void main()
{
	Operation::controlPC();
}