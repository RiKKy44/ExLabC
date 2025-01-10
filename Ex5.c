#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

//Helper function used for various test ensuring correct implementation.
void check(int cond, const char* errmsg)
{
	if (!cond)
	{
		fprintf(stderr, "Check failed: %s!", errmsg);
		exit(EXIT_FAILURE);
	}
}

typedef struct Card
{
	unsigned char suit; // values 1-4 denote (in order) Spades, Clubs, Diamonds, Hearts
	unsigned char rank; // values 9-14 denote (in order): 9, 10, J, Q, K, A
} Card;

typedef struct DeckElem
{
	Card c;
	struct DeckElem *next, *prev;
} DeckElem;

typedef struct Deck
{
	DeckElem *head, *tail;
} Deck;

Card card_init(unsigned char s, unsigned char r)
{
	Card c;
	c.suit = s;
	c.rank = r;
	return c;
}

Card card_random()
{
	return card_init(rand() % 4 + 1, rand() % 6 + 9);
}

void card_print(Card c)
{
	check(c.suit > 0 && c.suit <= 4, "card_print - Invalid suit!");
	check(c.rank > 8 && c.rank <= 14, "card_print - Invalid rank");
	static const char suit_lookup[] = { 'S', 'C', 'D', 'H' };
	static const char rank_lookup[] = { 'J', 'Q', 'K', 'A' };
	putchar('[');
	if (c.rank > 10)
		putchar(rank_lookup[c.rank - 11]);
	else
		printf("%hhu", c.rank);
	putchar(suit_lookup[c.suit - 1]);
	putchar(']');
}



#define MAX_HANDS 10
void deck_shuffle(Deck *d);


DeckElem* deck_create_elem(Card c) {
    DeckElem *el = malloc(sizeof(DeckElem));
    if (el == NULL) {
        return NULL; // Handle allocation failure
    }
    el->c = c;
    el->next = NULL;
    el->prev = NULL;
    return el;
}


void deck_print(const Deck *d) {
    if (d == NULL || d->head == NULL) {
        return;
    }
    DeckElem *el = d->head;
    while (el != NULL) {
        card_print(el->c); // Assuming card_print is implemented and works correctly
        el = el->next;
    }
}

void deck_destroy(Deck *d) {
    if (d == NULL) { // Check for NULL to avoid segmentation faults
        return;
    }
    DeckElem *el = d->head;
    while (el != NULL) {
        DeckElem *next = el->next;
        free(el); // Free the current element
        el = next;
    }
    d->head = NULL; // Set deck head and tail to NULL after freeing
    d->tail = NULL;
}
void deck_append_elem(Deck *d, DeckElem *el) {
    if (d == NULL || el == NULL) {
        return;
    }
    if (d->tail == NULL) { // Handle empty deck case
        d->head = el;
        d->tail = el;
        el->next = NULL;
        el->prev = NULL;
    } else {
        d->tail->next = el;
        el->prev = d->tail;
        el->next = NULL;
        d->tail = el;
    }
}

DeckElem * deck_unlink_last(Deck *d){
    if (d==NULL||d->tail == NULL){
        return NULL;
    }
    DeckElem *temp = d->tail;
    d->tail = d->tail->prev;
     if (d->tail != NULL) {  // If there is still a previous element
        d->tail->next = NULL; // Set the next pointer of the new tail to NULL
    } else { // If the list becomes empty
        d->head = NULL; // Set the head to NULL as well
    }
    return temp;
}
void deck_reverse(Deck *d) {
    if (d == NULL || d->head == NULL) { // Check for NULL deck or empty deck
        return;
    }

    DeckElem *element = d->head;
    DeckElem *temp = NULL;
    while (element != NULL) {
        // Swap next and prev pointers
        temp = element->next;
        element->next = element->prev;
        element->prev = temp;

        // Move to the next element (which is now the prev pointer)
        element = element->prev;
    }

    // Swap head and tail
    temp = d->head;
    d->head = d->tail;
    d->tail = temp;
}
void deck_prepend(Deck *dst, Deck *src) {
    if (src == NULL || src->head == NULL) {
        return; // If src is empty, do nothing
    }

    // If dst is not empty, link the tail of src to the head of dst
    if (dst->head != NULL) {
        src->tail->next = dst->head;
        dst->head->prev = src->tail;
    } else {
        // If dst is empty, just point the head and tail of dst to the head of src
        dst->head = src->head;
        dst->tail = src->tail;
    }

    // Update the head of dst to the head of src
    dst->head = src->head;

    // Set src's head and tail to NULL since it has been moved to dst
    src->head = NULL;
    src->tail = NULL;
}
Deck deck_build() {
    Deck deck = { NULL, NULL }; // Initialize an empty deck
    unsigned char suits[] = { 1, 2, 3, 4 }; // Spades, Clubs, Diamonds, Hearts
    unsigned char ranks[] = { 9, 10, 11, 12, 13, 14 }; // 9, 10, J, Q, K, A

    // Create a card for each combination of suit and rank
    for (int s = 0; s < 4; ++s) {
        for (int r = 0; r < 6; ++r) {
            DeckElem *el = deck_create_elem(card_init(suits[s], ranks[r]));
            if (deck.head == NULL) {
                // If the deck is empty, set head and tail to the first element
                deck.head = el;
                deck.tail = el;
            } else {
                // Append the element to the tail of the deck
                deck.tail->next = el;
                el->prev = deck.tail;
                deck.tail = el;
            }
        }
    }

    return deck;
}
void deck_deal(Deck *d, Deck hands[], unsigned n) {
    if (d == NULL || n == 0) {
        return; // Handle invalid input
    }

    unsigned hand_index = 0; // Start with the first hand
    while (d->tail != NULL) {
        // Unlink the last card from the deck
        DeckElem *card = deck_unlink_last(d);

        // Add the card to the current hand
        if (hands[hand_index].head == NULL) {
            // If the hand is empty, initialize it
            hands[hand_index].head = card;
            hands[hand_index].tail = card;
            card->next = NULL;
            card->prev = NULL;
        } else {
            // Append the card to the tail of the current hand
            card->next = NULL;
            card->prev = hands[hand_index].tail;
            hands[hand_index].tail->next = card;
            hands[hand_index].tail = card;
        }

        // Move to the next hand in a round-robin fashion
        hand_index = (hand_index + 1) % n;
    }
}
int main(void)
{
	//For Part 1 remember to uncomment two additional lines between Parts 2 and 3
	
	puts("**********************Part 1**********************");
	Deck d = { NULL, NULL };
	int i;
	srand(9854);
	//srand((unsigned)time(NULL));
	for (i = 0; i < 10; ++i)
	{
		DeckElem *tmp = deck_create_elem(card_random());
		deck_append_elem(&d, tmp);
	}
	deck_print(&d);
	

	/*
	puts("\n**********************Part 2**********************");
	for (i = 0; i < 4; ++i)
	{
		DeckElem *tmp = deck_unlink_last(&d);
		printf("Last: ");
		card_print(tmp->c);
		putchar('\n');
		free(tmp);
	}

	puts("Reversing the rest...");
	deck_reverse(&d);
	deck_print(&d);
	*/

	//Uncomment the following for Part 1
	
	deck_destroy(&d);
	check(d.head == NULL && d.tail == NULL, "Deck should be empty");
	

	/*
	puts("\n**********************Part 3**********************");
	for (i = 0; i < 3; ++i)
	{
		Deck d2 = { NULL, NULL };
		int j = rand() % 10 + 1;
		while (j--)
		{
			DeckElem *tmp = deck_create_elem(card_random());
			deck_append_elem(&d2, tmp);
		}
		printf("d2: ");
		deck_print(&d2);
		puts("Prepending d2 to d");
		deck_prepend(&d, &d2);
		printf("d : ");
		deck_print(&d);
		check(d2.head == NULL && d2.tail == NULL, "Second deck should be empty");
	}
	deck_destroy(&d);

	puts("Building deck...");
	d = deck_build();
	deck_print(&d);
	*/

	/*
	puts("\n**********************Part 4**********************");
	puts("Dealing cards...");
	Deck hands[3] = { {NULL, NULL} };
	deck_deal(&d, hands, 3);
	for (i = 0; i < 3; ++i)
	{
		printf("Hand %d: ", i + 1);
		deck_print(hands + i);
		deck_destroy(hands + i);
	}
	check(d.head == NULL && d.tail == NULL, "First deck should be empty");
	*/

	/*
	puts("\n**********************Part 5**********************");
	d = deck_build();
	deck_print(&d);
	puts("Shuffling 10 times...");
	for (i = 0; i < 10; ++i)
		deck_shuffle(&d);
	deck_print(&d);
	deck_destroy(&d);
	*/

	return 0;
}