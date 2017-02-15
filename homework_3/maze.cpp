bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er,
                int ec) {
  if(sr == er && sc == ec) return true;
  maze[sr][sc] = '%';

  if (sr > 0 /* there is a row above */ &&
      maze[sr - 1][sc] != 'X' /* can move north */ &&
      maze[sr - 1][sc] != '%' /* have not already visited that spot */) {
    if(pathExists(maze, nRows, nCols, sr - 1, sc, er, ec)) return true;
    maze[sr - 1][sc] = '%';
  }

  if (sc < nCols - 1 /* there is a column to the right */ &&
      maze[sr][sc + 1] != 'X' /* can move east */ &&
      maze[sr][sc + 1] != '%' /* have not already visited that spot */) {
    if(pathExists(maze, nRows, nCols, sr, sc + 1, er, ec)) return true;
    maze[sr][sc + 1] = '%';
  }

  if (sr < nRows - 1 /* there is a row below */ &&
      maze[sr + 1][sc] != 'X' /* can move south */ &&
      maze[sr + 1][sc] != '%' /* have not already visited that spot */) {
    if(pathExists(maze, nRows, nCols, sr + 1, sc, er, ec)) return true;
    maze[sr + 1][sc] = '%';
  }

  if (sc > 0 /* there is a column to the left */ &&
      maze[sr][sc - 1] != 'X' /* can move west */ &&
      maze[sr][sc - 1] != '%' /* have not already visited that spot */) {
    if(pathExists(maze, nRows, nCols, sr, sc - 1, er, ec)) return true;
    maze[sr][sc - 1] = '%';
  }

  return false;
}
