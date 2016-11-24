#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <bits/stdc++.h>
using namespace std;
#define WHITE 1.0, 1.0, 1.0
#define DESTINATION 1.0, 0.0, 0.0
#define OBSTACLE 0.545, 0.271, 0.075
#define BACKGROUND 1.000, 0.980, 0.980
//0.902, 0.902, 0.980
#define CURRENT 0.1, 0.2, 0.4
#define SOURCE 1.000, 1.000, 0.000
#define VISITED 0.678, 0.847, 0.902
#define FINAL_PATH 0.000, 0.392, 0.000
#define FINAL_VISITED 0.196, 0.804, 0.196
#define BORDER 0.0, 0.0, 0.0

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
// Global Variables
char title[] = "Maze";
int refreshMillis = 300;                 // Refresh period in milliseconds
ii source = ii(-1, -1), destination = ii(-1, -1);
bool taking_input = false, source_input = false, dest_input = false;
vvi maze, visited;
set<ii> final_visited;
vector<ii> shortest_path, exploration;
const int ROCK = -1, GEM = 1, FREE = 0;

GLfloat BORDER_WIDTH = 2.0;
GLint window_width = 700;
GLint window_height = 500;
GLfloat left_pos = 0.0;
GLfloat right_pos = 1.0;
GLfloat bottom_pos = 0.0;
GLfloat top_pos = 1.0;
GLint MAZE_WIDTH;
GLint MAZE_HEIGHT;
typedef vector<double> vd;
vd visited_color(3);
// Global variables
double STRAIGHT = 1;
double DIAGONAL = sqrt(2);
state NIL = mp(-1, -1);

vector<ii> offsets = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1,
		1 }, { 0, 1 }, { 1, 1 } }; //E, SE, S, SW, W, NW, N, NE


vd get_next_visited_color(vd temp){
    vd ret(3);
    ret[0] = ((double) rand() / (RAND_MAX));
    ret[1] = ((double) rand() / (RAND_MAX));
    ret[2] = ((double) rand() / (RAND_MAX));
    return ret;
}
bool valid(int i, int j, int r, int c) {
	if (i >= 0 && j >= 0 && i < r && j < c)
		return true;
	return false;
}

pair<vector<ii>, vector<ii> > findPathByDjikstra(ii s, ii d, vvi grid) {
	vector<ii> path;
	vector<ii> exploration;
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
		exploration.pb(pop);
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
			break;
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
	return mp(exploration, path);
}

double heuristic_estimate(ii node, ii goal) {
	return (double)(sqrt(pow(goal.F - node.F, 2) + pow(goal.S - node.S, 2)));
}
pair<vector<ii>, vector<ii> > findPathByAStar(ii s, ii d, vvi grid) {
	vector<ii> path, explore;
	priority_queue<pq_entry, vector<pq_entry>, greater<pq_entry> > pq;
	map<state, state_info> m;
	double iter = 0;
	pq.push(mp(0 + heuristic_estimate(s,d), s));
	iter++;
	m[s].cost = 0;
	m[s].parent = mp(-1, -1);
	while (!pq.empty()) {
		pq_entry front = pq.top();
		state pop = front.S;
		//explore.pb(pop);
		double pcost = front.F - heuristic_estimate(pop,d);
		if (pop == d) {
			state temp = d;
			while (1) {
				path.pb(temp);
				if (temp == s)
					break;
				temp = m[temp].parent;
			}
			reverse(path.begin(), path.end());

			break;
		}
		pq.pop();
		if (pcost > m[pop].cost)
			continue;
		// add neighbors
		explore.pb(pop);
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
						pq.push(mp(path_cost + heuristic_estimate(child ,d), child));
						iter++;
					}
				}

			}
		}

	}
	if (m.find(d) == m.end())
		cout << "No Path Found";
	cout << '\n';
	return mp(explore, path);
}
bool over = false;
bool dfs(ii u, double cost, double cost_limit , vector<ii>& path, vector<ii>& explored, ii source_loc,ii dest_loc,vvi grid_map) {
	if (cost > cost_limit) {
		return false;
	}
	if (u == dest_loc || over) {
		over = true;
		return true;
	}
    explored.pb(u);
	int r = grid_map.size();
	int c = grid_map[0].size();
	for (int i = 0; i < (int) offsets.size(); ++i) {
		ii v = ii(u.first + offsets[i].first, u.second + offsets[i].second);
		if (valid(u.first + offsets[i].first, u.second + offsets[i].second, r, c)) {
			if (grid_map[u.first + offsets[i].first][u.second + offsets[i].second]
					!= -1) {
				double step_cost = DIAGONAL;
				if (offsets[i].first == 0 || offsets[i].second == 0)
					step_cost = STRAIGHT;
					path.pb(v);

					if (dfs(v, cost + step_cost, cost_limit , path, explored , source_loc, dest_loc ,grid_map)) {
						return true;
					}
					path.pop_back();
			}
		}
	}
	return false;
}
pair<vector<ii>, vector<ii> > iterative_lengthening(ii source_loc,ii dest_loc,vvi grid_map) {
	vector<ii> explored, path;
	for (double limit = 0;; limit += 1.0) {
		path.clear();
		//explored.clear();
		path.pb(source_loc);
		//explored.pb(source_loc);
		if (dfs(source_loc, 0, limit, path, explored , source_loc, dest_loc ,grid_map)) {
			break;
		}
	}
	over = false;
	return mp(explored, path);
}
void print_vector(vector<ii> path) {
	for(auto pos: path) {
        cout << "("<< pos.F<<","<< pos.S<< ") ";
	}
	cout << endl;
}
void print_pair(ii p) {
    printf("(%d,%d)\n", p.F, p.S);
}
bool paused = false;
int move_number = 0;

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ii bot_position;
	bool explore_finished = false;
    if (!paused) {
        // Calculate animation parameters
        int b = shortest_path.size();
        int a = exploration.size();
        if(move_number >= (int)(a + b)){
            paused = true;
            explore_finished = true;
            bot_position = destination;
        } else if(move_number >= a) {
            if(move_number == a)
                cout << "Goal found!\nShowing the shortest path:-" << endl;
            explore_finished = true;
            bot_position=shortest_path[move_number - a];
            move_number++;
        } else {
            bot_position=exploration[move_number];
            move_number++;
        }
    }
	GLfloat xSize = (right_pos - left_pos) / MAZE_WIDTH;
	GLfloat ySize = (top_pos - bottom_pos) / MAZE_HEIGHT;
    vd next_visited_color = get_next_visited_color(visited_color);
	glBegin(GL_QUADS);
	for (GLint x = 0; x < MAZE_WIDTH; ++x) {
		for (GLint y = 0; y < MAZE_HEIGHT; ++y) {
            int value  = maze[y][x];
            if(taking_input){
               if(value == ROCK)
                    glColor3f(OBSTACLE);
               else if(x == source.S && y == source.F)
                    glColor3f(SOURCE);
               else if (value == GEM)
                    glColor3f(DESTINATION);
               else if (value == FREE){
                    glColor3f(BACKGROUND);
               }

            } else {
                if(explore_finished && x == bot_position.S && y == bot_position.F) {
                    print_pair(bot_position);
                    visited[y][x] = 1;
                    final_visited.insert(ii(y,x));
                    glColor3f(FINAL_PATH);
                } else if(explore_finished && final_visited.find(ii(y,x))!=final_visited.end()){
                    glColor3f(FINAL_VISITED);
                } else if(x == bot_position.S && y == bot_position.F){
                    //print_pair(bot_position);
                    visited[y][x] = 1;
                    glColor3f(CURRENT);
                } else if(x == source.S && y == source.F){
                    glColor3f(SOURCE);
                } else if (value == GEM) {
                    glColor3f(DESTINATION);
                } else if (visited[y][x] == 1){
                    glColor3f(VISITED);
                   // glColor3f(next_visited_color[0],next_visited_color[1],next_visited_color[2]);

                }
                else if (value == FREE)
                    glColor3f(BACKGROUND);
                else if (value == ROCK)
                    glColor3f(OBSTACLE);
            }
			glVertex2f(x * xSize + left_pos, y * ySize + bottom_pos); // S-W point
			glVertex2f((x + 1) * xSize + left_pos, y * ySize + bottom_pos); // S-E point
			glVertex2f((x + 1) * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-W point
			glVertex2f(x * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-E point
		}
	}
	glEnd();
	glColor3f(BORDER);
	glLineWidth(BORDER_WIDTH);
	for (GLint x = 0; x < MAZE_WIDTH; ++x) {
		for (GLint y = 0; y < MAZE_HEIGHT; ++y) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(x * xSize + left_pos, y * ySize + bottom_pos); // S-W point
			glVertex2f((x + 1) * xSize + left_pos, y * ySize + bottom_pos); // S-E point
			glVertex2f((x + 1) * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-W point
			glVertex2f(x * xSize + left_pos, (y + 1) * ySize + bottom_pos); // N-E point
            glEnd();
		}
	}
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
    case '1':
        if(taking_input && !source_input && dest_input) {
            dest_input = false;
            taking_input = false;
            printf("Starting Dijkstra...\n");
            pair<vector<ii>, vector<ii> > data = findPathByDjikstra(source, destination, maze);
            exploration = data.F;
            shortest_path = data.S;
            paused = false;
            visited_color[0] = 1;
            visited_color[1] = 1;
            visited_color[2] = 0;
        }
        break;
    case '2':
        if(taking_input && !source_input && dest_input) {
            dest_input = false;
            taking_input = false;
            printf("Starting A-Star...\n");
            pair<vector<ii>, vector<ii> > data = findPathByAStar(source, destination, maze);
            exploration = data.F;
            shortest_path = data.S;
            paused = false;
        }
        break;
    case '3':
        if(taking_input && !source_input && dest_input) {
            dest_input = false;
            taking_input = false;
            printf("Starting Iterative Lengthening...\n");
            pair<vector<ii>, vector<ii> > data = iterative_lengthening(source, destination, maze);
            exploration = data.F;
            shortest_path = data.S;
            paused = false;
        }
        break;
    case 'p':
    case 'P':
        paused = !paused;      // Toggle to opposite value
        if (!paused) {
            glutPostRedisplay();
        }
    break;
    case 'r':
    case 'R':
        move_number = 0;
        final_visited.clear();
        visited.assign(MAZE_HEIGHT, vi(MAZE_WIDTH, 0));
        paused = false;
        break;
    case 's':
    case 'S':
        paused = false;
        drawScene();
        paused = true;
        break;
    case 'f':
    case 'F':
        if(taking_input && !source_input && !dest_input) {
            printf("Right click on the source position, then press [F/f].\n");
            source_input = true;
        } else if(taking_input && source_input && !dest_input){
            source_input = false;
            dest_input = true;
            printf("Right click on the destination position, then press [F/f].\n");
        }
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
      if ( refreshMillis > 25) {          // Do not reduce less than 2.
         refreshMillis -= 20;
      }
      break;
   case GLUT_KEY_DOWN:
      if (refreshMillis<1000) {
         refreshMillis += 20;
      }
      break;
   }
}

void take_input() {
    printf("Rows: ");
    cin >> MAZE_HEIGHT;
    printf("Columns: ");
    cin >> MAZE_WIDTH;
    printf("Mark the positions of obstacles.");
    maze.assign(MAZE_HEIGHT, vi(MAZE_WIDTH, 0));
    visited.assign(MAZE_HEIGHT, vi(MAZE_WIDTH, 0));
    cout << " Task Finished? If Yes -> (Press F/f): " << endl;
    taking_input = true;
}

/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(BACKGROUND, 1.0); // Set background (clear) color to black
}

/* Callback handler for mouse event */
void mouse(int button, int state, int x, int y) {
    double cell_width = glutGet(GLUT_WINDOW_WIDTH) /(double) MAZE_WIDTH;
	double cell_height = glutGet(GLUT_WINDOW_HEIGHT) /(double) MAZE_HEIGHT;
    int row = (MAZE_HEIGHT - 1) - (int) (y / cell_height);
    int col = (int) (x / cell_width);

    //printf("Cell clicked: ");print_pair(ii(row, col));

    if(row < 0 || col < 0 || row >= MAZE_HEIGHT || col >= MAZE_WIDTH) {
        return;
    }
    if(taking_input && !source_input && !dest_input){
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
          if (maze[row][col] == ROCK) {
                maze[row][col] = FREE;
          } else {
                maze[row][col] = ROCK;
          }
        }
    } else if(taking_input && source_input){
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
          source = ii(row, col);
          printf("Source = ");
          print_pair(source);
        }
    } else if(taking_input && dest_input){
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
          maze[row][col] = GEM;
          destination = ii(row, col);
          printf("Destination = ");
          print_pair(destination);
          printf("\tPress '1' for Dijkstra Algorithm.\n");
          printf("\tPress '2' for A-Star Algorithm.\n");
        }
    }
}
void mouse_motion(int x, int y){
    double cell_width = glutGet(GLUT_WINDOW_WIDTH) /(double) MAZE_WIDTH;
	double cell_height = glutGet(GLUT_WINDOW_HEIGHT) /(double) MAZE_HEIGHT;
    int row = (MAZE_HEIGHT - 1) - (int) (y / cell_height);
    int col = (int) (x / cell_width);
    if(row < 0 || col < 0 || row >= MAZE_HEIGHT || col >= MAZE_WIDTH) {
        return;
    }
    if(taking_input && !source_input && !dest_input){
          if (maze[row][col] == ROCK) {
                maze[row][col] = FREE;
          } else {
                maze[row][col] = ROCK;
          }
          glutPostRedisplay();
    }
}
/* Called back when the timer expired */
void Timer(int value) {
    if(!paused)
        glutPostRedisplay();    // Post a paint request to activate display()
    glutTimerFunc(refreshMillis, Timer, 0); // subsequent timer call at milliseconds
}

int main(int argc, char **argv) {
    //initialize_grid();
    take_input();
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(title);                    // Create window with given title
	glutReshapeFunc(reshape);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(myKeyboardFunc);           // Handles "normal" ASCII symbols
	glutSpecialFunc( mySpecialKeyFunc );        // Handles "special" keyboard keys
	glutMouseFunc(mouse);                       // Register callback handler for mouse event
	glutMotionFunc(mouse_motion);
	initGL();                                   // Our own OpenGL initialization
	glutTimerFunc(0, Timer, 0);                 // First timer call immediately
	glutMainLoop();                             // Enter event-processing loop
	return 0;
}
