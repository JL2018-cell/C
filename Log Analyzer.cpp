#include<iostream>
#include<algorithm>
#include<string>
#include<map>
#include<vector>

using namespace std;

struct Page {
    int id;
    string path;
    int counter;
    Page(int id, string path) {
        this->id = id;
        this->path = path;
        counter = 0;
    };
};

// This function can facilitate sorting
bool operator<(const Page & a, const Page & b) {
    return (a.id < b.id);
}

vector<Page> pages;

struct User {
    int id;
    vector<string> visits;
    User(int id) {
        this->id = id;
    };
    void add_visit(int page_id) {
        Page p(page_id, "");
        string s;
	sort(pages.begin(), pages.end());
        vector<Page>::iterator iter = lower_bound(pages.begin(), pages.end(), p);
        s=iter->path;
        if(iter->id == page_id) {
            //cout << "Add: " <<s<<endl;
            visits.push_back(s);
        }
    };
    int size() const {
        return visits.size();
    };
    void print_visits() {
        sort(visits.begin(), visits.end());
        vector<string>::iterator iter;
        for(iter = visits.begin(); iter != visits.end(); iter++) {
            cout << "- " << *iter << endl;
        }
    }
};

vector<User> users;

// Please implement the following function to facilitate the sorting of users
bool operator<(const User & a, const User & b) {
 //cout << "a size="<<a.size() << " b size "<<b.size() <<endl;
  return (a.size()>b.size()); 
}

// Please implement the following function
void add_page(const Page& p) {
   pages.push_back(p);
}

// Please implement the following function
bool cmp_page_count(const Page & a, const Page & b) {
    if (a.counter>b.counter) return true;
    if (a.counter<b.counter) return false;
    return (a.path<b.path);
}

bool cmp_user_id (const User & a, const User & b) {
  if (a.size()<b.size()) return false;
  if (a.size()>b.size()) return true;
  return (a.id<b.id);
}
/*
bool cmp_page_path(const Page & a, const Page & b) {
    return (a.path>b.path);
}
*/
// Please implement the following function
void print_pages(int number) {
    int i=0;
    for (i=0;i<pages.size() && i<number;i++) {
      cout<<pages[i].counter<<":"<<pages[i].path<<endl;
    }
}

// Please implement the following function
void add_user(User u) {
    users.push_back(u);
}

// Please implement the following function
void add_visit(int user_id,int page_id) {
    for (auto & element : users) { 
        if(user_id==element.id)
          element.add_visit(page_id);
    }
    //for (int i=0;i<users.size();i++) {
    //    users[i].add_visit(page_id);
    //}
}

// Please implement the following function
void print_users(int number) {
    sort(users.begin(), users.end());    
    for (int i=0;i<users.size() && i<number;i++) {
      cout<<users[i].size()<<":"<<users[i].id<<endl;
      users[i].print_visits();
    }
}



int main() {

    string type;
    int user_id;
    while(cin >> type) {
        if(type == "USER") {
          cin >> user_id;
          /*
          if (user_id==12815) {
            cout<<"Found 12815."<<endl;
          }
          */
          User u(user_id);
          add_user(u);
        } else if(type == "PAGE") {
          int page_id;
          string page_path;
          cin >> page_id;
          cin >> page_path;
          Page p(page_id, page_path);
          add_page(p);
        } else if(type == "VISIT") {
          int page_id;
          cin >> page_id;
          Page p(page_id, "");
	  sort(pages.begin(), pages.end());
          vector<Page>::iterator iter = lower_bound(pages.begin(), pages.end(), p);
          if(iter->id == p.id) {
            iter->counter++;
          }
          add_visit(user_id, p.id);
        }
    }
    sort(pages.begin(), pages.end(), cmp_page_count);
    cout << "*** 5 most popular pages ***" << endl;
    print_pages(5);
    //sort(pages.begin(), pages.end());

    sort(users.begin(), users.end(), cmp_user_id);
    cout << "*** 5 most active users ***" << endl;
    print_users(5);

    return 0;

}

