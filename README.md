# The Unisex Bathroom Problem

## Problem
A woman was working in a cubicle in the basement of a concrete monolith, and
the nearest women’s bathroom was two floors up. She proposed to the Uberboss
that they convert the men’s bathroom on her floor to a unisex bathroom, sort
of like on Ally McBeal.

The Uberboss agreed, provided that the following synchronization constraints can be maintained:
• There cannot be men and women in the bathroom at the same time.
• There should never be more than three employees squandering company
time in the bathroom.

Of course the solution should avoid deadlock.

## Solution
- Two semaphores to control the number of people using the bathroom: one for men and another for women.
- A turnstile to prevent starvation, ensuring that individuals of the opposite sex can use the bathroom.
- A condition variable to release threads of the opposite sex once the current threads have vacated the bathroom.

## Differential
Most solutions to this problem utilize a light switch structure to release threads of the opposite sex. However, we opted to use condition variables to create a more elegant and distinct solution.

## Starvation
Without the turnstile, threads of the opposite sex would experience starvation. To illustrate this, we have two versions of the code: one with the turnstile and one without.