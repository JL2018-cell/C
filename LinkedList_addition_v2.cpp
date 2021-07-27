/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        int tmp, result;
        tmp = 0;
        ListNode* rs = l1;
        while (l1 != nullptr or l2 != nullptr) {
            if (l1 != nullptr) {
                result = l1->val;                
                l1 = l1->next;
            }
            if (l2 != nullptr) {
                result += l2->val;
                l2 = l2->next;
            }            
            if (tmp) {
                result += + 1;
                tmp = 0;
            }            
            if (result>9) {
                tmp = 1;
                result = result - 10;
                if (l1->next == nullptr and tmp != 0) {
                    l1->next = new ListNode();
                    l1->val = result;
                }
                else {
                    l1->val = result;
                }
            }
            else {
                tmp = 0;
                if (l1->next == nullptr) {
                    l1->next = new ListNode();
                }
                l1->val = result;
            }
        }
        return rs;
    }
};