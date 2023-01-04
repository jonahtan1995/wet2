

#ifndef WET2_AVLTREE_H
#define WET2_AVLTREE_H

#include <stdexcept>
using namespace std;

template <typename T, typename N>
class Node
{
public:
    T m_element;
    N m_key;
    int m_height;
    Node<T,N> *m_parent;
    Node<T,N> *m_left_son;
    Node<T,N> *m_right_son;
    int num_subtree;

    // bool operator==(const Node &other) const;
    Node(const T &data_element, N key, int height = 0, Node<T,N> *parent = nullptr, Node<T,N> *left_son = nullptr,
         Node<T,N> *right_son = nullptr);
    ~Node();
    // bool Node<T,N>::operator==(const Node &other) const;
};

/**************Node functions*****************/

template <class T, class N>
Node<T,N>::Node(const T &data_element, N key, int height, Node<T,N> *parent, Node<T,N> *left_son,
              Node<T,N> *right_son) : m_element(data_element), m_key(key), m_height(height), m_parent(parent), m_left_son(left_son), m_right_son(right_son), num_subtree(1) {}

template <class T, class N>
Node<T,N>::~Node()
{
    delete m_left_son;
    delete m_right_son;
    m_parent = nullptr;
    //delete m_element;
    //delete m_parent;
}

/**************end of Node functions*******/

template <class T, class N>
class AVLtree
{
public:
    Node<T,N> *m_root;
    N m_highest_key;
    int num_elements;

    AVLtree() : m_root(nullptr),m_highest_key(-1), num_elements(0){};
    ~AVLtree();
    Node<T,N> *clearTree(Node<T,N> *current_root);
    int calcHeight(Node<T,N> *m_node);
    int calcSubTree(Node<T,N> *node);
    int getBalance(Node<T,N> *m_node);
    Node<T,N> *findNode(Node<T,N> *root, N key);
    Node<T,N> *insertNode(Node<T,N> *root, N key, const T &data_element);
    Node<T,N> *RotateLL(Node<T,N> *node);
    Node<T,N> *RotateRR(Node<T,N> *node);
    Node<T,N> *RotateRL(Node<T,N> *node);
    Node<T,N> *RotateLR(Node<T,N> *node);
    Node<T,N> *deleteNode(Node<T,N> *node, N key );
    Node<T,N> *minValueNode(Node<T,N> *node);
    Node<T,N> *maxValueNode(Node<T,N> *node);
    int inOrderVisit(Node<T,N> *node, int *const array, int start_index);
    int inOrderVisitUnite(Node<T,N> *node, Node<T,N> **array, int start_index);
    Node<T,N> *closestLowerNode(Node<T,N> *root,Node<T,N> *node);
    Node<T,N>* select(Node<T,N>* root, int k);
    int numSubtree(Node<T,N>* node);

    int max(int a, int b)
    {
        return (a > b) ? a : b;
    }

    int abs(int a, int b)
    {
        if ((a - b) < 0)
        {
            return b - a;
        }
        return a - b;
    }
};

/**************AVLtree functions***************/
template <class T, class N>
AVLtree<T,N>::~AVLtree()
{
    m_root = clearTree(m_root);
    delete m_root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::clearTree(Node<T,N> *current_root)
{
    if (current_root)
    {
        current_root->m_left_son = clearTree(current_root->m_left_son);
        current_root->m_right_son = clearTree(current_root->m_right_son);
        delete current_root;
    }
    return nullptr;
}

template <class T, class N>
int AVLtree<T,N>::calcHeight(Node<T,N> *node)
{
    int h = 0;
    if (!node)
    {
        return -1;
    }
    else
    {
        if (node->m_left_son){
            if (node->m_right_son){
                h = 1 + max(node->m_left_son->m_height, node->m_right_son->m_height);
            }
            else {
                h = 1 + node->m_left_son->m_height;
            }
        }
        else if(node->m_right_son) {
            if (node->m_left_son) {
                h = 1 + max(node->m_left_son->m_height, node->m_right_son->m_height);
            }
            else {
                h = 1 + node->m_right_son->m_height;
            }
        }
    }
    return h;
}

template <class T, class N>
int AVLtree<T,N>::calcSubTree(Node<T,N> *node){
    if (!node){
        return 0;
    }
    return calcSubTree(node->m_left_son) + calcSubTree(node->m_right_son) + 1;
}


template <class T, class N>
int AVLtree<T,N>::getBalance(Node<T,N> *node)
{
    if (!node)
    {
        return 0;
    }
    return calcHeight(node->m_left_son) - calcHeight(node->m_right_son);
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::findNode(Node<T,N> *root, N key)
{
    if (key < 0)
    {
        return nullptr;
    }
    if (!root)
    {
        return nullptr;
    }
    if (root->m_key==key)
        {
        return root;

        }

    else if (root->m_key < key)
    {
        return findNode(root->m_right_son, key);
    }
    else if (root->m_key > key)
    {
        return findNode(root->m_left_son, key);
    }
    return nullptr;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::insertNode(Node<T,N> *root, N key, const T &data_element)
{
    if (root == nullptr)
    {
        try {
            root = new Node<T,N>(data_element, key);
        }
        catch (std::bad_alloc &){
            throw;
        }
        if (this->m_highest_key < key)
        {
            this->m_highest_key = key;
        }
        num_elements++;
    }
    else {
        if (key < root->m_key) {
            Node<T,N> *new_left = insertNode(root->m_left_son, key, data_element);
            if (new_left) {
                new_left->m_parent = root;
            }
            root->m_left_son = new_left;
        } else if (key > root->m_key) {
            Node<T,N> *new_right = insertNode(root->m_right_son, key, data_element);
            if (new_right) {
                new_right->m_parent = root;
            }
            root->m_right_son = new_right;
        }
    }

    if (this->m_highest_key < key)
    {
        this->m_highest_key = key;
    }
    root->m_height = 1 + max(calcHeight(root->m_left_son),
                             calcHeight(root->m_right_son));
    root->num_subtree = calcSubTree(root);

    int balance_factor = getBalance(root);
    if (balance_factor == 2 && getBalance(root->m_left_son) == 1)
    {
        // LL ROTATION
        return RotateLL(root);
    }
    else if (balance_factor == -2 && getBalance(root->m_right_son) == -1)
    {
        // RR ROTATION
        return RotateRR(root);
    }
    else if (balance_factor == -2 && getBalance(root->m_right_son) == 1)
    {
        // RL ROTATION
        return RotateRL(root);
    }
    else if (balance_factor == 2 && getBalance(root->m_left_son) == -1)
    {
        // LR ROTATION
        return RotateLR(root);
    }
    return root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::RotateLL(Node<T,N> *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }
    Node<T,N> *temp;
    Node<T,N> *new_root;
    temp = node;
    new_root = temp->m_left_son;
    temp->m_left_son = new_root->m_right_son;
    new_root->m_right_son = temp;
    new_root->m_parent = temp->m_parent;
    temp->m_parent = new_root;
    if (new_root->m_left_son) {
        new_root->m_left_son->m_parent = new_root;
    }
    if (new_root->m_parent) {
        if (new_root->m_key < new_root->m_parent->m_key) {
            new_root->m_parent->m_left_son = new_root;
        } else {
            new_root->m_parent->m_right_son = new_root;
        }
    }

    new_root->m_right_son->m_height = 1 + max(calcHeight(new_root->m_right_son->m_left_son),
                                              calcHeight(new_root->m_right_son->m_right_son));
    new_root->m_right_son->num_subtree = calcSubTree(new_root->m_right_son);
    new_root->m_height = 1 + max(calcHeight(new_root->m_left_son),
                                 calcHeight(new_root->m_right_son));
    new_root->num_subtree = calcSubTree(new_root);
    return new_root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::RotateRR(Node<T,N> *node)
{

    if (node == nullptr)
    {
        return nullptr;
    }
    Node<T,N> *temp;
    Node<T,N> *new_root;
    temp = node;

    new_root = temp->m_right_son;
    temp->m_right_son = new_root->m_left_son;
    new_root->m_left_son = temp;
    new_root->m_parent = temp->m_parent;
    temp->m_parent = new_root;
    if (new_root->m_right_son) {
        new_root->m_right_son->m_parent = new_root;
    }
    //new_root->m_left_son = temp;

    if (new_root->m_parent) {
        if (new_root->m_key < new_root->m_parent->m_key) {
            new_root->m_parent->m_left_son = new_root;
        } else {
            new_root->m_parent->m_right_son = new_root;
        }
    }
    new_root->m_left_son->m_height = 1 + max(calcHeight(new_root->m_left_son->m_left_son),
                                             calcHeight(new_root->m_left_son->m_right_son));
    new_root->m_left_son->num_subtree = calcSubTree(new_root->m_left_son);
    new_root->m_height = 1 + max(calcHeight(new_root->m_left_son),
                                 calcHeight(new_root->m_right_son));
    new_root->num_subtree = calcSubTree(new_root);
    return new_root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::RotateLR(Node<T,N> *node)
{
    node->m_left_son = RotateRR(node->m_left_son);
    node->m_left_son->m_height = 1 + max(calcHeight(node->m_left_son->m_left_son),
                                         calcHeight(node->m_left_son->m_right_son));
    node->m_left_son->num_subtree = calcSubTree(node->m_left_son);
    Node<T,N> *new_root = RotateLL(node);
    new_root->m_height = 1 + max(calcHeight(new_root->m_left_son),
                                 calcHeight(new_root->m_right_son));
    new_root->num_subtree = calcSubTree(new_root);
    return new_root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::RotateRL(Node<T,N> *node)
{
    node->m_right_son = RotateLL(node->m_right_son);
    node->m_right_son->m_height = 1 + max(calcHeight(node->m_right_son->m_left_son),
                                          calcHeight(node->m_right_son->m_right_son));
    node->m_right_son->num_subtree = calcSubTree(node->m_right_son);

    Node<T,N> *new_root = RotateRR(node);
    new_root->m_height = 1 + max(calcHeight(new_root->m_left_son),
                                 calcHeight(new_root->m_right_son));
    new_root->num_subtree = calcSubTree(new_root);
    return new_root;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::minValueNode(Node<T,N> *node)
{
    Node<T,N> *curr = node;

    while (curr->m_left_son != nullptr)
        curr = curr->m_left_son;

    return curr;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::maxValueNode(Node<T,N> *node)
{
    Node<T,N> *curr = node;

    while (curr->m_right_son != nullptr)
        curr = curr->m_right_son;

    return curr;
}

template <class T, class N>
Node<T,N> *AVLtree<T,N>::deleteNode(Node<T,N> *root, N key) // add a check for the best player
{
    if (root == nullptr) {
        return nullptr;
    }

    if (key < root->m_key) {
        root->m_left_son = deleteNode(root->m_left_son, key);
        if (root->m_left_son) {
            root->m_left_son->m_parent = root;
        }
    }
    else if (key > root->m_key) {
        root->m_right_son = deleteNode(root->m_right_son, key);
        if (root->m_right_son) {
            root->m_right_son->m_parent = root;
        }
    }
    else { // this means the key is equal to the current node's key
        if (key == this->m_highest_key) {
            Node<T, N> *temp = closestLowerNode(this->m_root, root);
            if (temp) {
                this->m_highest_key = temp->m_key;
            }
            temp = nullptr;
        }
        if (root->m_left_son && root->m_right_son) { // two children
            // we find the next node that should be the new root - this is the node with the smallest value in the right son tree
            Node<T, N> *new_root = minValueNode(root->m_right_son);
            // we copy the data element and the key to the root, but keep the height and pointers to the next sons.
            // this means we changed the value of the root node without actually deleting the root
            root->m_key = new_root->m_key;
            root->m_element = new_root->m_element;
            if (root->m_right_son->m_key == new_root->m_key) {
                new_root->m_parent = nullptr;
            }
            // now we delete the original node that was copied to become the root, we don't need it anymore because we copied its values
            root->m_right_son = deleteNode(root->m_right_son, root->m_key);
            if (root->m_right_son) {
                root->m_right_son->m_parent = root;
            }
        } else { // one or zero children
            Node<T, N> *temp = root;
            Node<T, N> *temp_parent = root->m_parent;
            if (root) {
                if (root->m_left_son == nullptr && root->m_right_son == nullptr) {
                    if (temp_parent) {
                        if (root->m_key < root->m_parent->m_key) {
                            temp_parent->m_left_son = nullptr;
                        } else {
                            temp_parent->m_right_son = nullptr;
                        }
                    }
                    delete temp;
                    num_elements--;
                    return nullptr;
                } else if (root->m_right_son == nullptr) {
                    root = root->m_left_son;
                    if (temp_parent) {
                        if (temp->m_key < temp->m_parent->m_key) {
                            temp->m_parent->m_left_son = root;
                        } else {
                            temp->m_parent->m_right_son = root;
                        }
                    }
                    if (root) {
                        root->m_parent = temp_parent;
                    }
                    temp->m_left_son = nullptr;
                    delete temp;
                    num_elements--;
                } else if (temp->m_left_son == nullptr) {
                    root = root->m_right_son;
                    if (temp_parent) {
                        if (temp->m_key < temp->m_parent->m_key) {
                            temp->m_parent->m_left_son = root;
                        } else {
                            temp->m_parent->m_right_son = root;
                        }
                    }
                    if (root) {
                        root->m_parent = temp_parent;
                    }
                    temp->m_right_son = nullptr;
                    delete temp;
                    num_elements--;
                }
            }
        }
    }
        root->m_height = 1 + max(calcHeight(root->m_left_son),
                                 calcHeight(root->m_right_son));
        root->num_subtree = calcSubTree(root);


        int balance_factor = getBalance(root);
        if (balance_factor == 2 && getBalance(root->m_left_son) == 1) {
            // LL ROTATION
            return RotateLL(root);
        } else if (balance_factor == -2 && getBalance(root->m_right_son) == -1) {
            // RR ROTATION
            return RotateRR(root);
        } else if (balance_factor == -2 && getBalance(root->m_right_son) == 1) {
            // RL ROTATION
            return RotateRL(root);
        } else if (balance_factor == 2 && getBalance(root->m_left_son) == -1) {
            // LR ROTATION
            return RotateLR(root);
        }

        return root;

}

template <class T, class N>
int AVLtree<T,N>::inOrderVisit(Node<T,N> *node, int *const array, int start_index)
{
    if (!node)
    {
        return start_index;
    }
    else
    {
        int index_after_left_visit = inOrderVisit(node->m_left_son, array, start_index);
        array[index_after_left_visit] = node->m_key;
        int index_after_right_visit = inOrderVisit(node->m_right_son, array, index_after_left_visit + 1);
        return index_after_right_visit;
    }
}


template <class T, class N>
Node<T,N> *AVLtree<T,N>::closestLowerNode(Node<T,N> *root,Node<T,N> *node)
{
    if (!node || !root)
    {
        return nullptr;
    }
    if (!node->m_parent && !node->m_left_son)
    {
        // this node doesn't have a parent or a left son - there is no lower value node in the tree
        return nullptr;
    }
    if (node->m_parent && node->m_left_son == nullptr)
    {
        // there is a parent and no left son
        if (node->m_key > node->m_parent->m_key)
        {
            // this node is the right son of the parent, so the parent is smaller
            return node->m_parent;
        }
        else{
            //this node is the left son of the parent so it is smaller than the parent
            if (node->m_key > root->m_key) {
                //this node is to the right of the root
                Node<T,N> *minUnderRootRight = minValueNode(root->m_right_son);
                if (minUnderRootRight != nullptr) {
                    if (node->m_key == minUnderRootRight->m_key) {
                        return root;
                    } else {
                        return closestLowerNode(root->m_right_son, node);
                    }
                }
            }
        }
    }
    else
    {
        // there is a left son and a parent or only a left son and no parent, either way the parent is not the closest lower
        return maxValueNode(node->m_left_son);
    }
    return nullptr;
}



template <class T, class N>
int AVLtree<T,N>::inOrderVisitUnite(Node<T,N> *node, Node<T,N> **array, int start_index)
{
    if (!node)
    {
        return start_index;
    }
    int index_after_left_visit = inOrderVisitUnite(node->m_left_son, array, start_index);
    array[index_after_left_visit] = node;
    int index_after_right_visit = inOrderVisitUnite(node->m_right_son, array, index_after_left_visit + 1);
    //delete node;
    return index_after_right_visit;
}
template<class T,class N>
int AVLtree<T,N>::numSubtree(Node<T,N>* node){
    if (!node){
        return 0;
    }
    return node->num_subtree;
}


template<class T,class N>
Node<T,N> *AVLtree<T,N>:: select(Node<T,N>* root, int k){
        int leftSonSubtree = numSubtree(root->m_left_son);
        if(leftSonSubtree == k-1)
        {
            return root;
        }
        else if(leftSonSubtree > k-1)
        {
            return select(root->m_left_son,k);
        }
        else
        {
            int newK = k - leftSonSubtree - 1;
            return select(root->m_right_son,newK);
        }
}

/**************end of AVLtree functions***************/



#endif //WET2_AVLTREE_H
