# Embedded Interview Series
## Understanding static in C – global `static` elements

As I continue exploring embedded systems concepts, today I'll focus on another powerful aspect of the static keyword in C: global static variables and functions.


#### The Explanation:
When applied at the global scope, the static keyword takes on a different but equally important role—it restricts the visibility of variables and functions to the file they're defined in.

### Key Characteristics of Global Static Elements:
- File scope only (not visible outside their translation unit)
- Static global variables retain their values throughout program execution
- Static functions can only be called within the file they're defined
- Protected from name collisions across multiple files
- Help enforce encapsulation in C programming



### Why This Matters:
- Reduced Symbol Table Size: Fewer exported symbols means faster linking and smaller binaries.
- Better Information Hiding: Prevents unintended access to implementation details.
- Module-like Organization: Allows for clean separation of concerns even in C.
- Reduced Name Collisions: Avoids namespace pollution in larger projects.