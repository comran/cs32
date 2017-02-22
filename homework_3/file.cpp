class File {
 public:
  File(std::string name);
  virtual ~File();
  virtual std::string name() const;
  virtual void open() const = 0;
  virtual void redisplay() const;

 private:
  std::string filename_;
};

File::File(std::string filename) : filename_(filename) {}

File::~File() {}

std::string File::name() const { return filename_; }

void File::redisplay() const { std::cout << "refresh the screen"; }

class Picture : public File {
 public:
  Picture(std::string filename);
  virtual ~Picture();
  virtual void open() const;
};

Picture::Picture(std::string filename) : File(filename) {}

Picture::~Picture() {
  std::cout << "Destroying the picture " << name() << std::endl;
}

void Picture::open() const { std::cout << "show picture"; }

class Video : public File {
 public:
  Video(std::string filename, int duration);
  virtual ~Video();
  virtual void open() const;
  virtual void redisplay() const;

 private:
  int duration_;
};

Video::Video(std::string filename, int duration)
    : File(filename), duration_(duration) {}

Video::~Video() {
  std::cout << "Destroying " << name() << ", a video" << std::endl;
}

void Video::open() const {
  std::cout << "play " << duration_ << " second video";
}

void Video::redisplay() const { std::cout << "replay video"; }

class TextMsg : public File {
 public:
  TextMsg(std::string filename);
  virtual ~TextMsg();
  virtual void open() const;
};

TextMsg::TextMsg(std::string filename) : File(filename) {}

TextMsg::~TextMsg() {
  std::cout << "Destroying " << name() << ", a text message" << std::endl;
}

void TextMsg::open() const { std::cout << "open text message"; }
