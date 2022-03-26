#include <cassert>
#include <deque>
#include <iostream>

#include "test_runner.h"

using namespace std;
// Structure of binary tree node
struct Node {
    Node(int v, Node* p)
            : value(v)
            , parent(p)
    {}

    int value;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent;
};
// Needed function itself
Node* Next(Node* me) {
    // In BST next by value element will be deepest left descendant of a right descendant of current node
    if (me->right != nullptr) {
        me = me->right;
        while (me->left != nullptr)
            me = me->left;
    }
        // So if there is no right descendant, we should go up, until reaching the node with value bigger than current.
        // It will be the node, which left branch will be the branch we climbed from.
        // Or null, if this was the biggest value in whole tree
    else {
        while (me->parent != nullptr && me != me->parent->left)
            me = me->parent;
        me = me->parent;
    }

    return me;
}
// Tests, provided by authors
class NodeBuilder {
public:
    Node* CreateRoot(int value) {
        nodes.emplace_back(value, nullptr);
        return &nodes.back();
    }

    Node* CreateLeftSon(Node* me, int value) {
        assert( me->left == nullptr );
        nodes.emplace_back(value, me);
        me->left = &nodes.back();
        return me->left;
    }

    Node* CreateRightSon(Node* me, int value) {
        assert( me->right == nullptr );
        nodes.emplace_back(value, me);
        me->right = &nodes.back();
        return me->right;
    }

private:
    deque<Node> nodes;
};

void Test1() {
    NodeBuilder nb;

    Node* root = nb.CreateRoot(50);
    ASSERT_EQUAL( root->value, 50 )

    Node* l = nb.CreateLeftSon(root, 2);
    Node* min = nb.CreateLeftSon(l, 1);
    Node* r = nb.CreateRightSon(l, 4);
    ASSERT_EQUAL( min->value, 1)
    ASSERT_EQUAL( r->parent->value, 2 )

    nb.CreateLeftSon(r, 3);
    nb.CreateRightSon(r, 5);

    r = nb.CreateRightSon(root, 100);
    l = nb.CreateLeftSon(r, 90);
    nb.CreateRightSon(r, 101);

    nb.CreateLeftSon(l, 89);
    r = nb.CreateRightSon(l, 91);

    ASSERT_EQUAL( Next(l)->value, 91)
    ASSERT_EQUAL( Next(root)->value, 89 )
    ASSERT_EQUAL( Next(min)->value, 2 )
    ASSERT_EQUAL( Next(r)->value, 100)

    while (min) {
        cout << min->value << '\n';
        min = Next(min);
    }
}

void TestRootOnly() {
    NodeBuilder nb;
    Node* root = nb.CreateRoot(42);
    ASSERT( Next(root) == nullptr)
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, Test1);
    RUN_TEST(tr, TestRootOnly);
    return 0;
}
