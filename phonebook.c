#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct person {
    char name[20];
    char mobile[20];
    char home[20];
    char office[20];
    char email[50];
    char group[10];
} person;

typedef struct contact {
    person *data;
    int size;
    int capacity;
} contact;

#define FILE_NAME "contact.txt"

#define ADD '1'
#define VIEW '2'
#define CALL '3'
#define MODIFY '4'
#define DELETE '5'
#define SEARCH '6'
#define SAVE '7'
#define EXIT '0'

void init_contact(contact *c);
void free_contact(contact *c);
void load_contact(contact *c);
void save_contact(contact *c);
int is_empty(contact *c);
int is_full(contact *c);
void expand_contact(contact *c);
void add_person(contact *c);
void view_person(contact *c);
void call_person(contact *c);
void modify_person(contact *c);
void delete_person(contact *c);
int search_by_name(contact *c, char *name);
int search_by_phone(contact *c, char *phone);
void show_person(person p);
void show_welcome();
void show_menu();
char get_choice();
void clear_buffer();
void clear_screen();

int main() {
    contact c;
    char choice;

    init_contact(&c);
    load_contact(&c);
    show_welcome();

    while (1) {
        show_menu();
        choice = get_choice();

        switch (choice) {
            case ADD:
                add_person(&c);
                break;

            case VIEW:
                view_person(&c);
                break;

            case CALL:
                call_person(&c);
                break;

            case MODIFY:
                modify_person(&c);
                break;

            case DELETE:
                delete_person(&c);
                break;

            case SEARCH: {
                char key[20];
                int index;

                printf("Enter name or phone number to search: ");
                scanf("%s", key);
                clear_buffer();

                index = search_by_name(&c, key);
                if (index == -1) {
                    index = search_by_phone(&c, key);
                }

                if (index == -1) {
                    printf("No matching record found!\n");
                } else {
                    show_person(c.data[index]);
                }
                break;
            }

            case SAVE:
                save_contact(&c);
                break;

            case EXIT:
                printf("Thank you for using the system. Goodbye!\n");
                free_contact(&c);
                return 0;

            default:
                printf("Invalid option. Please try again.\n");
        }

        printf("Press Enter to return to the main menu...\n");
        getchar();
        clear_screen();
    }
}

void init_contact(contact *c) {
    c->data = NULL;
    c->size = 0;
    c->capacity = 0;
}

void free_contact(contact *c) {
    if (c->data != NULL) {
        free(c->data);
        c->data = NULL;
    }
    c->size = 0;
    c->capacity = 0;
}

void load_contact(contact *c) {
    FILE *fp;
    int n;
    int i;

    fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("Failed to open file!\n");
        return;
    }

    fscanf(fp, "%d\n", &n);

    if (n > 0) {
        c->data = (person *)malloc(n * sizeof(person));
        if (c->data == NULL) {
            printf("Memory allocation failed!\n");
            fclose(fp);
            return;
        }

        for (i = 0; i < n; i++) {
            person p;
            fscanf(fp, "%s %s %s %s %s %s\n",
                   p.name, p.mobile, p.home,
                   p.office, p.email, p.group);
            c->data[i] = p;
        }

        c->size = n;
        c->capacity = n;
    }

    fclose(fp);
}

void save_contact(contact *c) {
    FILE *fp;
    int i;

    fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("Failed to open file!\n");
        return;
    }

    fprintf(fp, "%d\n", c->size);

    for (i = 0; i < c->size; i++) {
        person p = c->data[i];
        fprintf(fp, "%s %s %s %s %s %s\n",
                p.name, p.mobile, p.home,
                p.office, p.email, p.group);
    }

    fclose(fp);
    printf("Data saved successfully!\n");
}

int is_empty(contact *c) {
    return c->size == 0;
}

int is_full(contact *c) {
    return c->size == c->capacity;
}

void expand_contact(contact *c) {
    if (is_full(c)) {
        int new_capacity;
        person *new_data;

        new_capacity = c->capacity + 10;
        new_data = (person *)realloc(c->data, new_capacity * sizeof(person));

        if (new_data == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }

        c->data = new_data;
        c->capacity = new_capacity;
    }
}

void add_person(contact *c) {
    person p;

    expand_contact(c);

    printf("Enter name: ");
    scanf("%s", p.name);
    clear_buffer();

    printf("Enter mobile number: ");
    scanf("%s", p.mobile);
    clear_buffer();

    printf("Enter home phone: ");
    scanf("%s", p.home);
    clear_buffer();

    printf("Enter office phone: ");
    scanf("%s", p.office);
    clear_buffer();

    printf("Enter email: ");
    scanf("%s", p.email);
    clear_buffer();

    printf("Enter group: ");
    scanf("%s", p.group);
    clear_buffer();

    if (strlen(p.mobile) == 0 && strlen(p.home) == 0 && strlen(p.office) == 0) {
        printf("At least one phone number must be provided!\n");
        return;
    }

    c->data[c->size] = p;
    c->size++;

    printf("Contact added successfully!\n");
}

void view_person(contact *c) {
    char q[10];
    int i;
    int found = 0;

    if (is_empty(c)) {
        printf("Contact list is empty!\n");
        return;
    }

    printf("Enter group name: ");
    scanf("%s", q);
    clear_buffer();

    for (i = 0; i < c->size; i++) {
        if (strcmp(c->data[i].group, q) == 0) {
            if (!found) {
                printf("Name            Mobile              Home                Office\n");
                found = 1;
            }
            printf("%-16s%-20s%-24s%-20s\n",
                   c->data[i].name,
                   c->data[i].mobile,
                   c->data[i].home,
                   c->data[i].office);
        }
    }

    if (!found) {
        printf("No such group found.\n");
    }
}

void call_person(contact *c) {
    int i;
    int index;
    person p;
    char *phone;

    if (is_empty(c)) {
        printf("Contact list is empty!\n");
        return;
    }

    printf("Select a contact to call:\n");

    for (i = 0; i < c->size; i++) {
        printf("%d. %s\n", i + 1, c->data[i].name);
    }

    scanf("%d", &index);
    clear_buffer();

    if (index < 1 || index > c->size) {
        printf("Invalid number!\n");
        return;
    }

    p = c->data[index - 1];

    if (strlen(p.mobile) > 0) phone = p.mobile;
    else if (strlen(p.home) > 0) phone = p.home;
    else phone = p.office;

    printf("Dialing %s...\n", p.name);

    for (i = 0; i < strlen(phone); i++) {
        printf("%c", phone[i]);
        fflush(stdout);
        usleep(100000);
    }

    printf("\n");
}

void modify_person(contact *c) {
    int i;
    int index;
    person *p;

    if (is_empty(c)) {
        printf("Contact list is empty!\n");
        return;
    }

    printf("Select a contact to modify:\n");

    for (i = 0; i < c->size; i++) {
        printf("%d. %s\n", i + 1, c->data[i].name);
    }

    scanf("%d", &index);
    clear_buffer();

    if (index < 1 || index > c->size) {
        printf("Invalid number!\n");
        return;
    }

    p = &c->data[index - 1];

    printf("Enter new name (%s): ", p->name);
    scanf("%s", p->name); clear_buffer();

    printf("Enter new mobile (%s): ", p->mobile);
    scanf("%s", p->mobile); clear_buffer();

    printf("Enter new home (%s): ", p->home);
    scanf("%s", p->home); clear_buffer();

    printf("Enter new office (%s): ", p->office);
    scanf("%s", p->office); clear_buffer();

    printf("Enter new email (%s): ", p->email);
    scanf("%s", p->email); clear_buffer();

    printf("Enter new group (%s): ", p->group);
    scanf("%s", p->group); clear_buffer();

    if (strlen(p->mobile) == 0 && strlen(p->home) == 0 && strlen(p->office) == 0) {
        printf("At least one phone number must be provided!\n");
        return;
    }

    printf("Contact updated successfully!\n");
}

void delete_person(contact *c) {
    int i;
    int index;

    if (is_empty(c)) {
        printf("Contact list is empty!\n");
        return;
    }

    printf("Select a contact to delete:\n");

    for (i = 0; i < c->size; i++) {
        printf("%d. %s\n", i + 1, c->data[i].name);
    }

    scanf("%d", &index);
    clear_buffer();

    if (index < 1 || index > c->size) {
        printf("Invalid number!\n");
        return;
    }

    for (i = index - 1; i < c->size - 1; i++) {
        c->data[i] = c->data[i + 1];
    }

    c->size--;
    printf("Contact deleted successfully!\n");
}

int search_by_name(contact *c, char *name) {
    int i;
    for (i = 0; i < c->size; i++) {
        if (strcmp(c->data[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int search_by_phone(contact *c, char *phone) {
    int i;
    for (i = 0; i < c->size; i++) {
        if (strcmp(c->data[i].mobile, phone) == 0 ||
            strcmp(c->data[i].home, phone) == 0 ||
            strcmp(c->data[i].office, phone) == 0) {
            return i;
        }
    }
    return -1;
}

void show_person(person p) {
    printf("Name: %s\n", p.name);
    printf("Mobile: %s\n", p.mobile);
    printf("Home: %s\n", p.home);
    printf("Office: %s\n", p.office);
    printf("Email: %s\n", p.email);
    printf("Group: %s\n", p.group);
}

void show_welcome() {
    printf("Welcome to the Contact Management System!\n");
    printf("(Press Enter to continue)\n");
    clear_buffer();
    clear_screen();
}

void show_menu() {
    printf("Select an option:\n");
    printf("1. Add Contact\n");
    printf("2. View Contacts by Group\n");
    printf("3. Call Contact\n");
    printf("4. Modify Contact\n");
    printf("5. Delete Contact\n");
    printf("6. Search Contact\n");
    printf("7. Save Contacts\n");
    printf("0. Exit\n");
}

char get_choice() {
    char choice;
    scanf("%c", &choice);
    clear_buffer();
    return choice;
}

void clear_buffer() {
    while (getchar() != '\n');
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[1;1H");
#endif
}
