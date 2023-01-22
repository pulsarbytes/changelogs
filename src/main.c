/*
 * Changelogs - A simple command line program to document all notable changes for a list of projects
 * by Yannis Maragos - https://www.pulsarbytes.net
 *
 * v1.0.1
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2020 Yannis Maragos
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXPROJECTS 100
#define MAXLINE 100
#define MAXVERSIONLINE 1000
#define MAXVERSIONNUMBER 100

enum state_codes
{
    MENU,
    ABOUT,
    CLOSE,
    OPEN_PROJECT,
    ADD_PROJECT,
    DELETE_PROJECT,
    ADD_VERSION,
    DELETE_VERSION
};

enum messages_codes
{
    NO_MESSAGE,
    PROJECT_ADDED,
    PROJECT_DELETED,
    VERSION_ADDED,
    VERSION_DELETED
};

char *project_name = NULL;
int state = MENU;
int message = NO_MESSAGE;
char *projects[MAXPROJECTS];

// Array of messages must be in sync with enum messages_codes
char *messages[] = {
    "",
    "Project added.",
    "Project deleted.",
    "Version added.",
    "Version deleted."};

void header(void);
void showmessages(void);
void menu(void);
void about(void);
void close(void);
void openproject(void);
void addproject(void);
void deleteproject(void);
void addversion(void);
void deleteversion(void);
char *selectproject(void);
void listversions(void);
void nametofilename(char *, char *);
void cleanup(void);

int main(void)
{
    // Array of function pointers must be in sync with enum state_codes
    void (*states[])() = {
        menu,
        about,
        close,
        openproject,
        addproject,
        deleteproject,
        addversion,
        deleteversion};
    void (*fn)(void);

    while (1)
    {
        header();

        if (state != OPEN_PROJECT)
        {
            showmessages();
        }

        fn = states[state];
        fn();
    }

    return 0;
}

void showmessages(void)
{
    // Show message if it exists
    if (strlen(messages[message]) > 0)
    {
        printf("\n%s\n\n", messages[message]);

        // Reset message
        message = NO_MESSAGE;
    }
}

void cleanup(void)
{
    int i;

    for (i = 0; i < MAXPROJECTS; i++)
    {
        if (projects[i] != NULL)
        {
            free(projects[i]);
        }
    }

    printf("Resources freed up.\n");
}

void close(void)
{
    cleanup();

    printf("Program terminated.\n");
    exit(0);
}

void header(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("Changelogs\n");
    printf("----------------------------------------\n");
}

void menu(void)
{
    char option = '\0';

    project_name = NULL;

    printf("Main menu\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("%-5s %35s", "(o)", "Open project\n");
    printf("%-5s %35s", "(a)", "Add project\n");
    printf("%-5s %35s", "(d)", "Delete project\n");
    printf("%-5s %35s", "(b)", "About\n");
    printf("%-35s %5s", "(x)", "Exit\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter your selection: ");

    while (scanf("%c", &option) != 1 ||
           (tolower(option) != 'o' &&
            tolower(option) != 'a' &&
            tolower(option) != 'd' &&
            tolower(option) != 'b' &&
            tolower(option) != 'x'))
    {
        printf("Enter your selection: ");

        while (getchar() != '\n')
            ;
    }

    // We have a char, read through any leftover chars
    while (getchar() != '\n')
        ;

    option = tolower(option);

    switch (option)
    {
    case 'o':
        state = OPEN_PROJECT;
        break;
    case 'a':
        state = ADD_PROJECT;
        break;
    case 'd':
        state = DELETE_PROJECT;
        break;
    case 'b':
        state = ABOUT;
        break;
    case 'x':
        state = CLOSE;
        break;
    default:
        state = MENU;
    }
}

void openproject(void)
{
    char option = '\0';

    // If no project selected, go back to menu
    if (project_name == NULL && (project_name = selectproject()) == NULL)
    {
        state = MENU;

        return;
    }

    header();
    showmessages();

    printf("Project: %s", project_name);
    printf("----------------------------------------\n");

    // List versions
    listversions();

    printf("----------------------------------------\n");
    printf("\n");
    printf("%-5s %35s", "(a)", "Add version\n");
    printf("%-5s %35s", "(d)", "Delete version\n");
    printf("%-5s %35s", "(o)", "Open other project\n");
    printf("%-5s %35s", "(m)", "Main menu\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter your selection: ");

    while (scanf("%c", &option) != 1 ||
           (tolower(option) != 'a' &&
            tolower(option) != 'd' &&
            tolower(option) != 'o' &&
            tolower(option) != 'm'))
    {
        printf("Enter your selection: ");

        while (getchar() != '\n')
            ;
    }

    // We have a char, read through any leftover chars
    while (getchar() != '\n')
        ;

    option = tolower(option);

    switch (option)
    {
    case 'a':
        state = ADD_VERSION;
        break;
    case 'd':
        state = DELETE_VERSION;
        break;
    case 'o':
        project_name = NULL;
        state = OPEN_PROJECT;
        break;
    case 'm':
        state = MENU;
        break;
    default:
        state = OPEN_PROJECT;
        break;
    }
}

void nametofilename(char *name, char *file_name)
{
    int i = 0, len = strlen(name);

    // Remove new line and spaces from project name
    while (i < len)
    {
        if (name[i] == '\n')
        {
            break;
        }

        if (name[i] == ' ')
        {
            file_name[i] = '-';
        }
        else
        {
            file_name[i] = tolower(name[i]);
        }

        i++;
    }

    file_name[i] = '\0';
    strcat(file_name, ".txt");
}

void listversions(void)
{
    FILE *fp;
    int count = 0;
    char file_name[MAXLINE];
    char version[MAXVERSIONLINE];

    nametofilename(project_name, file_name);

    if ((fp = fopen(file_name, "r")) == NULL)
    {
        // Create file
        fp = fopen(file_name, "w");
    }

    // Print versions
    while (fgets(version, MAXVERSIONLINE, fp))
    {
        if (version[0] == '-')
        {
            printf("  %s", version);
        }
        else
        {
            count++;
            printf("\n%s", version);
        }
    }

    if (count == 0)
    {
        printf("\n");
        printf("There are no versions yet.\n");
    }

    printf("\n");

    fclose(fp);
}

char *selectproject(void)
{
    FILE *fp;
    int j, i = 0, max = 0, option = 0;
    char option_c = '\0';
    char line[MAXLINE];
    char *project;

    if ((fp = fopen("projects.txt", "r")) == NULL)
    {
        // Create projects.txt file
        fp = fopen("projects.txt", "w");
    }

    // Store projects to array
    while (fgets(line, MAXLINE, fp) && max++ < MAXPROJECTS - 1)
    {
        if ((project = (char *)malloc(strlen(line))) == NULL)
        {
            fprintf(stderr, "Could not allocate space for project name %s\n", line);
        }

        strcpy(project, line);
        projects[i++] = project;
    }

    // List projects
    if (i > 0)
    {
        printf("Open project\n");
        printf("----------------------------------------\n");
        printf("\n");

        for (j = 0; j < i; ++j)
        {
            printf("(%d) %37s", j + 1, projects[j]);
        }

        printf("\n");
        printf("(0) %37s", "Cancel\n");
    }
    else
    {
        printf("\n");
        printf("You have not created any projects yet.\n");
        printf("\n\n");
        printf("%-5s %35s", "(m)", "Main menu\n");
        printf("\n");
        printf("----------------------------------------\n");
        printf("Enter your selection: ");

        while (scanf("%c", &option_c) != 1 ||
               tolower(option_c) != 'm')
        {
            printf("Enter your selection: ");

            while (getchar() != '\n')
                ;
        }

        // We have a char, read through any leftover chars
        while (getchar() != '\n')
            ;

        switch (option_c)
        {
        case 'm':
            state = MENU;
            break;
        default:
            state = OPEN_PROJECT;
            break;
        }

        fclose(fp);

        return NULL;
    }

    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter your selection: ");

    while (scanf("%d", &option) != 1 || option < 0)
    {
        printf("Enter your selection: ");

        while (getchar() != '\n')
            ;
    }

    // We have a char, read through any leftover chars
    while (getchar() != '\n')
        ;

    fclose(fp);

    // Cancel
    if (option == 0 || option > MAXPROJECTS)
    {
        return NULL;
    }
    else
    {
        return projects[option - 1];
    }
}

void addproject(void)
{
    FILE *fp;
    char project[MAXLINE];

    if ((fp = fopen("projects.txt", "a")) == NULL)
    {
        fprintf(stderr, "Could not open projects.txt to add new project.\n");
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }

    printf("New project\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("(0) %37s", "Cancel\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter project name: ");

    fgets(project, MAXLINE, stdin);

    if (strlen(project) == 0 || isspace(project[0]))
    {
        return;
    }

    // Cancel
    if (project[0] == '0')
    {
        fclose(fp);
        state = MENU;

        return;
    }

    // Write new project to file
    if (fputs(project, fp) >= 0)
    {
        fclose(fp);
        state = MENU;
        message = PROJECT_ADDED;
    }
    else
    {
        fprintf(stderr, "Could not save new project.\n");
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }
}

void deleteproject(void)
{
    FILE *fp, *temp;
    int j, i = 0, max = 0, option = 0;
    char option_c = '\0';
    char line[MAXLINE];
    char *project_name;
    char file_name[MAXLINE];

    if ((fp = fopen("projects.txt", "r")) == NULL)
    {
        // Create projects.txt file
        fp = fopen("projects.txt", "w");
    }

    // Store projects to array
    while (fgets(line, MAXLINE, fp) && max++ < MAXPROJECTS - 1)
    {
        if ((project_name = (char *)malloc(strlen(line))) == NULL)
        {
            fprintf(stderr, "Could not allocate space for project name %s\n", line);
        }

        strcpy(project_name, line);
        projects[i++] = project_name;
    }

    // List projects
    if (i > 0)
    {
        printf("Delete project\n");
        printf("----------------------------------------\n");
        printf("\n");

        for (j = 0; j < i; ++j)
        {
            printf("(%d) %37s", j + 1, projects[j]);
        }

        printf("\n");
        printf("(0) %37s", "Cancel\n");
    }
    else
    {
        printf("\n");
        printf("You have not created any projects yet.\n");
        printf("\n\n");
        printf("%-5s %35s", "(m)", "Main menu\n");
        printf("\n");
        printf("----------------------------------------\n");
        printf("Enter your selection: ");

        while (scanf("%c", &option_c) != 1 ||
               tolower(option_c) != 'm')
        {
            printf("Enter your selection: ");

            while (getchar() != '\n')
                ;
        }

        // We have a char, read through any leftover chars
        while (getchar() != '\n')
            ;

        switch (option_c)
        {
        case 'm':
            state = MENU;
            break;
        default:
            state = DELETE_PROJECT;
            break;
        }

        fclose(fp);

        return;
    }

    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter your selection: ");

    while (scanf("%d", &option) != 1 || option < 0)
    {
        printf("Enter your selection: ");

        while (getchar() != '\n')
            ;
    }

    // We have a char, read through any leftover chars
    while (getchar() != '\n')
        ;

    // Cancel
    if (option == 0 || option > MAXPROJECTS)
    {
        fclose(fp);
        state = MENU;

        return;
    }

    // Create temp file
    if ((temp = fopen("temp.txt", "w")) == NULL)
    {
        fprintf(stderr, "Could not create temp.txt.\n");
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }

    // Reset pointer to start of file
    rewind(fp);
    max = 0;

    while (fgets(line, MAXLINE, fp) && max++ < MAXPROJECTS - 1)
    {
        if (max != option)
        {
            fputs(line, temp);
        }
        else
        {
            nametofilename(line, file_name);

            // Remove project file
            remove(file_name);
        }
    }

    fclose(fp);
    remove("projects.txt");

    fclose(temp);
    rename("temp.txt", "projects.txt");
    project_name = NULL;

    message = PROJECT_DELETED;
}

void addversion(void)
{
    FILE *fp;
    char version_number[MAXVERSIONNUMBER];
    char note[MAXVERSIONLINE];
    char file_name[MAXLINE];

    nametofilename(project_name, file_name);

    if ((fp = fopen(file_name, "a")) == NULL)
    {
        fprintf(stderr, "Could not open %s to add new version.\n", file_name);
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }

    printf("Project: %s", project_name);
    printf("Add version\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("(x) %37s", "Cancel/End\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter version number: ");

    // Get version number from input
    fgets(version_number, MAXVERSIONNUMBER, stdin);

    if (strlen(version_number) == 0 || isspace(version_number[0]) || (!isdigit(version_number[0]) && tolower(version_number[0]) != 'x'))
    {
        return;
    }

    // Cancel
    if (tolower(version_number[0]) == 'x')
    {
        fclose(fp);
        state = OPEN_PROJECT;

        return;
    }

    // Write new version to file
    if (fputs(version_number, fp) >= 0)
    {
        printf("Enter note for this version: ");

        // Get notes from input
        while (fgets(note, MAXVERSIONLINE, stdin))
        {
            // Cancel/End
            if (tolower(note[0]) == 'x')
            {
                break;
            }
            else
            {
                if (note[0] != '\n')
                {
                    fputs("- ", fp);
                    fputs(note, fp);
                    printf("Enter note for this version: ");
                }
            }
        }

        fclose(fp);
        state = OPEN_PROJECT;
        message = VERSION_ADDED;
    }
    else
    {
        fprintf(stderr, "Could not save new version.\n");
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }
}

void deleteversion(void)
{
    FILE *fp, *temp;
    char version_number[MAXVERSIONNUMBER];
    char file_name[MAXLINE];
    char line[MAXLINE];
    int len, found = 0;

    nametofilename(project_name, file_name);

    if ((fp = fopen(file_name, "r")) == NULL)
    {
        fprintf(stderr, "Could not open %s to delete version.\n", file_name);
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }

    printf("Project: %s", project_name);
    printf("Delete version\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("(x) %37s", "Cancel\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter version number: ");

    // Get version number from input
    fgets(version_number, MAXVERSIONNUMBER, stdin);
    len = strlen(version_number);

    if (len == 0 || isspace(version_number[0]) || (!isdigit(version_number[0]) && tolower(version_number[0]) != 'x'))
    {
        return;
    }

    // Cancel
    if (tolower(version_number[0]) == 'x')
    {
        fclose(fp);
        state = OPEN_PROJECT;

        return;
    }

    // Create tempversions file
    if ((temp = fopen("tempversions.txt", "w")) == NULL)
    {
        fprintf(stderr, "Could not create tempversions.txt.\n");
        fprintf(stderr, "Program terminated.\n");
        exit(1);
    }

    // Read versions from file
    while (fgets(line, MAXLINE, fp))
    {
        if (strncmp(line, version_number, len) == 0)
        {
            found = 1;
        }
        else
        {
            if (strncmp(line, "- ", 2) == 0 && found == 1)
            {
                continue;
            }
            else
            {
                found = 0;
                fputs(line, temp);
            }
        }
    }

    fclose(fp);
    remove(file_name);

    fclose(temp);
    rename("tempversions.txt", file_name);

    state = OPEN_PROJECT;
    message = VERSION_DELETED;
}

void about(void)
{
    char option = '\0';

    printf("About\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("A simple command line program to document\n");
    printf("all notable changes for a list of projects.\n");
    printf("Developed by Yannis Maragos.\n");
    printf("Copyright (c) 2020 Yannis Maragos.\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("%-5s %35s", "(m)", "Main menu\n");
    printf("\n");
    printf("----------------------------------------\n");
    printf("Enter your selection: ");

    while (scanf("%c", &option) != 1 ||
           tolower(option) != 'm')
    {
        printf("Enter your selection: ");

        while (getchar() != '\n')
            ;
    }

    // We have a char, read through any leftover chars
    while (getchar() != '\n')
        ;

    option = tolower(option);

    switch (option)
    {
    case 'm':
        state = MENU;
        break;
    default:
        state = ABOUT;
        break;
    }
}
