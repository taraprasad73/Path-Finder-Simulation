#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <bits/stdc++.h>
using namespace std;
#define WHITE 1.0, 1.0, 1.0
#define BLACK 0.0, 0.0, 0.0
#define RED 1.0, 0.0, 0.0
#define GREEN 1.0, 1.0, 0.0
#define BLUE 0.0, 1.0, 0.0

#define F first
#define S second
#define pb push_back
#define mp make_pair
#define epsilon 1e-12

typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<vector<int> > vvi;
typedef ii state;
typedef pair<double, state> pq_entry;
typedef struct {
	double cost;
	ii parent;
} state_info;

// Global variables
double STRAIGHT = 1;
double DIAGONAL = sqrt(2);
state NIL = mp(-1, -1);

vector<ii> offsets = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1,
		1 }, { 0, 1 }, { 1, 1 } }; //E, SE, S, SW, W, NW, N, NE

bool valid(int i, int j, int r, int c) {
	if (i >= 0 && j >= 0 && i < r && j < c)
		return true;
	return false;
}

vector<ii> findPathByDjikstra(ii s, ii d, vvi grid) {
	vector<ii> path;
	priority_queue<pq_entry, vector<pq_entry>, greater<pq_entry> > pq;
	map<state, state_info> m;
	double iter = 0;
	pq.push(mp(0, s));
	iter++;
	m[s].cost = 0;
	m[s].parent = mp(-1, -1);
	while (!pq.empty()) {
		pq_entry front = pq.top();
		state pop = front.S;
		double pcost = front.F;
		if (pop == d) {
			state temp = d;
			while (1) {
				path.pb(temp);
				if (temp == s)
					break;
				temp = m[temp].parent;
			}
			reverse(path.begin(), path.end());
		}
		pq.pop();
		if (pcost > m[pop].cost)
			continue;
		// add neighbors
		for (int i = 0; i < (int) offsets.size(); i++) {
			int ni = pop.F + offsets[i].F;
			int nj = pop.S + offsets[i].S;
			if (valid(ni, nj, grid.size(), grid[0].size())) {
				if (grid[ni][nj] != -1) {
					state child;
					child.F = ni;
					child.S = nj;
					double path_cost = m[pop].cost;
					double delay = iter * epsilon;
					if (abs(offsets[i].F) == 1 && abs(offsets[i].S) == 1)
						path_cost += DIAGONAL;
					else
						path_cost += STRAIGHT;
					path_cost += delay;
					if (m.find(child) == m.end() || path_cost < m[child].cost) {
						m[child].cost = path_cost;
						m[child].parent = pop;
						pq.push(mp(path_cost, child));
						iter++;
					}
				}

			}
		}

	}
	if (m.find(d) == m.end())
		cout << "No Path Found";
	cout << '\n';
	return path;
}

// Global Variables
char title[] = "Maze";    // Windowed mode's title
int refreshMillis = 300;                 // Refresh period in milliseconds
ii source, destination;
vvi maze;
vector<ii> shortest_path;
const int ROCK = -1, GEM = 1, FREE = 0;

GLint FPS = 24;
GLint window_width = 700;
GLint window_height = 500;
GLfloat left_pos = 0.0;
GLfloat right_pos = 1.0;
GLfloat bottom_pos = 0.0;
GLfloat top_pos = 1.0;
GLint game_width = 4;
GLint game_height = 4;

void print_vector(vector<ii> path) {
	for(auto pos: path) {
        cout << "["<< pos.F<<","<< pos.S<< "] ";
	}
	cout << endl;
}
void print_pair(ii p) {
    printf("[%d,%d]\n", p.F, p.S);
}
int RunMode = 1;
int move_number = 0;
int AnimateStep = 2;

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ii bot_position;
    if (RunMode==1) {
        // Calculate animation parameters
        if(move_number >= (int)shortest_path.size()){
            move_number = 0;
        }
        bot_position=shortest_path[move_number];
        move_number++;
    }
	GLfloat xSize = (right_pos - left_pos) / game_width;
	GLfloat ySize = (top_pos - bottom_pos) / game_height;

	glBegin(GL_QUADS);
	for (GLint x = 0; x < game_width; ++x) {
		for (GLint y = 0; y < game_height; ++y) {
            int value  = maze[y][x];
            if(x == bot_position.S && y == bot_position.F){
                print_pair(bot_position);
                glColor3f(BLUE);
            }
             else if(x == source.S && y == source.F){
                glColor3f(GREEN);
            }
			else if (value == GEM) {
                glColor3f(BLACK);
            }
			else if (value == FREE)
				glColor3f(WHITE);
			else if (value == ROCK)
				glColor3f(RED);

			glVertex2f(x * xSize + left_pos, y * ySize + bottom_pos); // S-W point
			glVertex2f((x + 1) * xSize + left_pos, y * ySize + bottom_pos); // S-E point
			glVertex2f((x + 1) * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-W point
			glVertex2f(x * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-E point
		}
	}
	glEnd();
    // Flush the pipeline, swap the buffers
	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	window_width = w;
	window_height = h;
	glViewport(0, 0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left_pos, right_pos, bottom_pos, top_pos);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void myKeyboardFunc( unsigned char key, int x, int y )
{
   switch ( key ) {
   case 'r':
      RunMode = 1-RunMode;      // Toggle to opposite value
      if ( RunMode==1 ) {
         glutPostRedisplay();
      }
      break;
   case 's':
      RunMode = 1;
      drawScene();
      RunMode = 0;
      break;
   case 27:   // Escape key
      exit(1);
   }
}
// glutSpecialFunc is called below to set this function to handle
//      all "special" key presses.  See glut.h for the names of
//      special keys.
void mySpecialKeyFunc( int key, int x, int y )
{
   switch ( key ) {
   case GLUT_KEY_UP:
      if ( AnimateStep > 3) {          // Do not reduce less than 2.
         AnimateStep--;
      }
      break;
   case GLUT_KEY_DOWN:
      if (AnimateStep<1000) {
         AnimateStep++;
      }
      break;
   }
}

void initialize_grid() {
    ifstream fin;
    fin.open("input.txt");
    int m, n;
    fin >> m >> n;
    game_height = m;
    game_width = n;
    maze.assign(m, vi(n));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            fin >> maze[i][j];
            if(maze[i][j] == 1) {
                destination = ii(i,j);
                print_pair(destination);
            }
        }
    }
    fin >> source.first >> source.second;
    shortest_path = findPathByDjikstra(source, destination, maze);
    print_vector(shortest_path);
    fin.close();
}
/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(0.0, 0.0, 0.0, 1.0); // Set background (clear) color to black
}
/* Called back when the timer expired */
void Timer(int value) {
    if(RunMode==1)
        glutPostRedisplay();    // Post a paint request to activate display()
    glutTimerFunc(refreshMillis, Timer, 0); // subsequent timer call at milliseconds
}

void menu_message() {
    cout << "Arrow keys control speed.  Press \"r\" to run,  \"s\" to single step." << endl;
}

int main(int argc, char **argv) {
    initialize_grid();
    menu_message();

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(title);                    // Create window with given title
	glutReshapeFunc(reshape);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(myKeyboardFunc);           // Handles "normal" ASCII symbols
	glutSpecialFunc( mySpecialKeyFunc );        // Handles "special" keyboard keys
	initGL();                                   // Our own OpenGL initialization
	glutTimerFunc(0, Timer, 0);                 // First timer call immediately
	glutMainLoop();                             // Enter event-processing loop
	return 0;
}
