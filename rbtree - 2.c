


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED				1
#define BLACK 			2

typedef int KEY_TYPE;

typedef struct _rbtree_node {
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	void *value;
} rbtree_node;

typedef struct _rbtree {
	rbtree_node *root;
	rbtree_node *nil;
} rbtree;

rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x) {
	while (x->left != T->nil) {
		x = x->left;
	}
	return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x) {
	while (x->right != T->nil) {
		x = x->right;
	}
	return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x) {
	rbtree_node *y = x->parent;

	if (x->right != T->nil) {
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right)) {
		x = y;
		y = y->parent;
	}
	return y;
}


void rbtree_left_rotate(rbtree *T, rbtree_node *x) {

	rbtree_node *y = x->right;  // x  --> y  ,  y --> x,   right --> left,  left --> right
    rbtree_node *b = y->left;


	x->right = b; //1 1
	if (b != T->nil) { //1 2
        b->parent = x;
	}

	y->parent = x->parent; //1 3
	if (x->parent == T->nil) { //1 4
		T->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x; //1 5
	x->parent = y; //1 6
}


void rbtree_right_rotate(rbtree *T, rbtree_node *y) {

	rbtree_node *x = y->left;
    rbtree_node *b = x->right;

	y->left = b;
	if (b != T->nil) {
        b->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil) {
		T->root = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

/*
	 * 修复颜色，因为插入的点，我们总是将其先染成红色，所以如果父节点为黑色则不需要修复，如果父节点为红色，则需要修复
	 * 修复颜色分三种情况：
	 * ①当前插入点的父亲为红色，且祖父节点的另一个节点为也为红色，且父节点为祖父节点的左子节点
	 * ②当前插入点的父节点为红色，且祖父节点的另一个节点为黑色，且本节点为父结点的右子节点
	 * ③当前插入点的父节点为红色，且祖父节点的另一个节点为黑色，且本节点为父结点的左子节点
	 */

void rbtree_insert_fixup(rbtree *T, rbtree_node *z) {

	while (z->parent->color == RED) { //z ---> RED
		if (z->parent == z->parent->parent->left) {//如果z的父节点是祖父节点的左子节点
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED) {                          //情况1
				z->parent->color = BLACK;                   //情况1
				y->color = BLACK;                           //情况1
				z->parent->parent->color = RED;             //情况1
				z = z->parent->parent;                      //情况1
			} else {                                        //情况2
				if (z == z->parent->right) {                //情况2
					z = z->parent;                          //情况2
					rbtree_left_rotate(T, z);               //情况2
				}                                           //情况2
				z->parent->color = BLACK;                   //情况3
				z->parent->parent->color = RED;             //情况3
				rbtree_right_rotate(T, z->parent->parent);  //情况3
			}
		}else {//当父节点是祖父节点的右孩子的情况，由操作对称性原理可知，所需操作与上面差别在于将所有的left操作和节点改成对应的right的
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent; //z --> RED
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
		
	}

	T->root->color = BLACK;
}


void rbtree_insert(rbtree *T, rbtree_node *z) {

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil) {
		y = x;
		if (z->key < x->key) {
			x = x->left;
		} else if (z->key > x->key) {
			x = x->right;
		} else { //Exist
			return ;
		}
	}

	z->parent = y;
	if (y == T->nil) {
		T->root = z;
	} else if (z->key < y->key) {
		y->left = z;
	} else {
		y->right = z;
	}

	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
}

void rbtree_delete_fixup(rbtree *T, rbtree_node *x) {//x: child of node_for_delete
    if(x->color==RED)
    {
        //此处real D 为黑色,有3种情况，
        // 1） x 为 DL，DR 为 nil，实际删除结点 real D 为 原来需删除结点 z                         情况B)变种
        // 2） x 为 DR，DL 为 nil，实际删除结点 real D 为 原来需删除结点 z:                        情况B)
        // 3） x 为 DR，DL 为 nil，实际删除结点 real D 为 原来需删除结点 z 右子树 值相邻结点:        情况B)
        x->color=BLACK;//此时因删除黑色 real D 后，P左子树行高差1，只需把 x 变为黑色，补齐黑高，则可以满足红黑树要求。
        return ;
    }

    //以下均为 x 为 DR，x->color=BLACK 情况处理方法：                                           情况C) 情况D)
    rbtree_node *DR = x;
	while ((DR != T->root) && (DR->color == BLACK)) {
		if (DR == DR->parent->left) {

			rbtree_node *S= DR->parent->right;
			if (S->color == RED) {//将情况C) 转 情况D)
                S->color = BLACK;                       //情况C)
                DR->parent->color = RED;                //情况C)
				rbtree_left_rotate(T, DR->parent);      //情况C)
                S = DR->parent->right;                  //情况C)
			}
            ////////////////////情况D)////////////////////////////////
			if (S->left->color == RED ) {               //情况 D.1
                S->left->color = DR->parent->color;     //情况 D.1
                S->color = BLACK;                       //情况 D.1
                DR->parent->color = BLACK;              //情况 D.1
                rbtree_right_rotate(T, S);              //情况 D.1
                rbtree_left_rotate(T, DR->parent);      //情况 D.1
                DR = T->root;
            }
			else if (S->right->color == RED ) {         //情况 D.2
                S->color = DR->parent->color;           //情况 D.2
                S->right->color = BLACK;                //情况 D.2
                DR->parent->color = BLACK;              //情况 D.2
                rbtree_left_rotate(T, DR->parent);      //情况 D.2
            }
			else {
                if (DR->parent->color == RED) {         //情况 D.3 ,SL、SR都为黑色(其实都为Nil节点)，P为红色的情况
                    S->color = RED;                     //情况 D.3
                    DR->parent->color = BLACK;          //情况 D.3
                    DR = T->root;                       //情况 D.3
                } else {                                //情况 D.4 ,SL、SR都为黑色(其实都为Nil节点)，P为黑色的情况
                    S->color = RED;                     //情况 D.4
                    DR = DR->parent;                    //情况 D.4
                }
            }
			/*
//            if ((S->left->color == BLACK) && (S->right->color == BLACK)) {
//                S->color = RED;
//                DR = DR->parent;
//            } else {
//				if (S->right->color == BLACK) {
//                    S->left->color = BLACK;
//                    S->color = RED;
//					rbtree_right_rotate(T, S);
//                    S = DR->parent->right;
//				}
//
//                S->color = DR->parent->color;
//                DR->parent->color = BLACK;
//                S->right->color = BLACK;
//				rbtree_left_rotate(T, DR->parent);

//                DR = T->root;
//			}*/

		} else {

			rbtree_node *S = DR->parent->left;
			if (S->color == RED) { //将情况C) 转 情况D)
                S->color = BLACK;                       //情况C)
                DR->parent->color = RED;                //情况C)
				rbtree_right_rotate(T, DR->parent);     //情况C)
                S = DR->parent->left;                   //情况C)
			}
            ////////////////////情况D)////////////////////////////////
            if (S->right->color == RED ) {               //情况 D.1
                S->right->color = DR->parent->color;     //情况 D.1
                S->color = BLACK;                       //情况 D.1
                DR->parent->color = BLACK;              //情况 D.1
                rbtree_left_rotate(T, S);              //情况 D.1
                rbtree_right_rotate(T, DR->parent);      //情况 D.1
                DR = T->root;
            }
            else if (S->left->color == RED ) {         //情况 D.2
                S->color = DR->parent->color;           //情况 D.2
                S->left->color = BLACK;                //情况 D.2
                DR->parent->color = BLACK;              //情况 D.2
                rbtree_right_rotate(T, DR->parent);      //情况 D.2
            }
            else {
                if (DR->parent->color == RED) {         //情况 D.3 ,SL、SR都为黑色(其实都为Nil节点)，P为红色的情况
                    S->color = RED;                     //情况 D.3
                    DR->parent->color = BLACK;          //情况 D.3
                    DR = T->root;                       //情况 D.3
                } else {                                //情况 D.4 ,SL、SR都为黑色(其实都为Nil节点)，P为黑色的情况
                    S->color = RED;                     //情况 D.4
                    DR = DR->parent;                    //情况 D.4
                }
            }



//			if ((S->left->color == BLACK) && (S->right->color == BLACK)) {
//                S->color = RED;
//                DR = DR->parent;
//			} else {
//
//				if (S->left->color == BLACK) {
//                    S->right->color = BLACK;
//                    S->color = RED;
//					rbtree_left_rotate(T, S);
//                    S = DR->parent->left;
//				}
//
//                S->color = DR->parent->color;
//                DR->parent->color = BLACK;
//                S->left->color = BLACK;
//				rbtree_right_rotate(T, DR->parent);
//
//                DR = T->root;
//			}

		}
	}

    DR->color = BLACK;
}

rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z) {

	rbtree_node *D = T->nil;//D: node_for_delete
	rbtree_node *x = T->nil;//x: child of node_for_delete

	if ((z->left == T->nil) || (z->right == T->nil)) {
        D = z;
        //1) z->left != T->nil  && z->right == T->nil 则 DL->color = RED
        //2) z->left == T->nil  && z->right != T->nil 则 DR->color = RED
        //3) z->left == T->nil  && z->right == T->nil 另讨论
	} else {
        D = rbtree_successor(T, z);
	}

	if (D->left != T->nil) {
		x = D->left;
	} else if (D->right != T->nil) {
		x = D->right;
	}

	x->parent = D->parent;
	if (D->parent == T->nil) {
		T->root = x;
	} else if (D == D->parent->left) {
        D->parent->left = x;
	} else {
        D->parent->right = x;
	}

	if (D != z) {//real D 的值覆盖  原本需删除结点z
		z->key = D->key;
		z->value = D->value;
	}

	if (D->color == BLACK) {//若真实删除结点D为红色，则为情况A),此处不进入rbtree_delete_fixup处理
		rbtree_delete_fixup(T, x);
	}
	else{//情况A)

	}

	return D;
}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key) {

	rbtree_node *node = T->root;
	while (node != T->nil) {
		if (key < node->key) {
			node = node->left;
		} else if (key > node->key) {
			node = node->right;
		} else {
			return node;
		}	
	}
	return T->nil;
}


void rbtree_traversal(rbtree *T, rbtree_node *node) {
	if (node != T->nil) {
		rbtree_traversal(T, node->left);
		printf("key:%d, color:%d\n", node->key, node->color);
		rbtree_traversal(T, node->right);
	}
}

int main() {

	int keyArray[20] = {24,25,13,35,23, 26,67,47,38,98, 20,19,17,49,12, 21,9,18,14,15};

	rbtree *T = (rbtree *)malloc(sizeof(rbtree));
	if (T == NULL) {
		printf("malloc failed\n");
		return -1;
	}
	
	T->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
	T->nil->color = BLACK;
	T->root = T->nil;

	rbtree_node *node = T->nil;
	int i = 0;
	for (i = 0;i < 20;i ++) {
		node = (rbtree_node*)malloc(sizeof(rbtree_node));
		node->key = keyArray[i];
		node->value = NULL;

		rbtree_insert(T, node);
	}

	rbtree_traversal(T, T->root);
	printf("----------------------------------------\n");

	for (i = 0;i < 20;i ++) {

		rbtree_node *node = rbtree_search(T, keyArray[i]);
		rbtree_node *cur = rbtree_delete(T, node);
		free(cur);

		rbtree_traversal(T, T->root);
		printf("----------------------------------------\n");
	}
	

	
}





