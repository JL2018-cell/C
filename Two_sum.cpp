class Solution {
public:
    int present_in(set <pair <int, int> > temp, int num) {
        set< pair<int, int> >::iterator it;
        for (it=temp.begin(); it!=temp.end(); ++it) {
            if ((*it).second == num) return (*it).first;
        }
        return -1;        
    }
    
    vector<int> twoSum(vector<int>& nums, int target) {
        int i, comp, state;
        vector <int> result;
        set <pair <int, int> > temp;
        for (i=0; i<nums.size(); i++) {
            comp = target - nums[i];
            state = present_in(temp, comp);
            if (state != -1) {
                result.push_back(i);
                result.push_back(state);
                return  result;
            }
            else temp.insert(make_pair(i, nums[i]));
        }
        return result;
    }
};