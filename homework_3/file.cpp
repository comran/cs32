#include <iostream>
#include <string>
using namespace std;

class File {
 public:
  File(string name);
  virtual ~File();
  virtual string name() const;
  virtual void open() const = 0;
  virtual void redisplay() const;

 private:
  string filename_;
};

File::File(string filename) : filename_(filename) {}

File::~File() {}

string File::name() const { return filename_; }

void File::redisplay() const { cout << "refresh the screen"; }

class Picture : public File {
 public:
  Picture(string filename);
  virtual ~Picture();
  virtual void open() const;
};

Picture::Picture(string filename) : File(filename) {}

Picture::~Picture() { cout << "Destroying the picture " << name() << endl; }

void Picture::open() const { cout << "show picture"; }

class Video : public File {
 public:
  Video(string filename, int duration);
  virtual ~Video();
  virtual void open() const;
  virtual void redisplay() const;

 private:
  int duration_;
};

Video::Video(string filename, int duration)
    : File(filename), duration_(duration) {}

Video::~Video() { cout << "Destroying " << name() << ", a video" << endl; }

void Video::open() const { cout << "play " << duration_ << " second video"; }

void Video::redisplay() const { cout << "replay video"; }

class TextMsg : public File {
 public:
  TextMsg(string filename);
  virtual ~TextMsg();
  virtual void open() const;
};

TextMsg::TextMsg(string filename) : File(filename) {}

TextMsg::~TextMsg() {
  cout << "Destroying " << name() << ", a text message" << endl;
}

void TextMsg::open() const { cout << "open text message"; }

void openAndRedisplay(const File *f) {
  cout << f->name() << ": ";
  f->open();
  cout << endl << "Redisplay: ";
  f->redisplay();
  cout << endl;
}

int main() {
  File *files[4];
  files[0] = new TextMsg("fromFred.txt");
  // Videos have a name and running time
  files[1] = new Video("goblin.mpg", 3780);
  files[2] = new Picture("kitten.jpg");
  files[3] = new Picture("baby.jpg");

  for (int k = 0; k < 4; k++) openAndRedisplay(files[k]);

  // Clean up the files before exiting
  cout << "Cleaning up." << endl;
  for (int k = 0; k < 4; k++) delete files[k];
}
