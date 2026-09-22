# Homework Assignment 1: answers

Four questions. A few sentences each is enough. Submit this file.

## Q1. Which representation did you choose, and why?

Name it (COO, CSR, CSC, or something else) and say what one lookup costs: given
a user id, how much work is it to list all of that user's friends?

## Q2. How much memory did you save?

The adjacency matrix for this dataset would be 4,039 x 4,039 = 16,313,521
entries. Count the integers your class actually allocates for
`facebook-combined.txt`. What fraction of the matrix is that?

## Q3. What does `most_popular_user` cost?

Write the running time in terms of $n$ (users) and $m$ (friendships), for your
representation. If you had picked a different one, would this answer change?

## Q4. What did the sanitizer find?

If it found something, what was the bug and what line did it point at. If it
found nothing, say what you did while writing the rule of three that kept you
out of trouble.
