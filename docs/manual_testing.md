# Manual Testing Checklist

## Initialization

- Delete `data/accounts.dat`, `data/categories.dat`, `data/foods.dat`
- Start the program
- Check that all three files are recreated automatically
- Check that demo accounts `admin` and `demo` exist

## Registration

- Register a new user with valid data
- Verify the user appears in `Show accounts`
- Try registering the same login again
- Expected result: validation error without crash

## Authorization

- Login with `admin / admin123`
- Login with `demo / demo123`
- Try wrong password
- Try non-existing login
- Expected result: controlled error, no crash

## Account View

- Login as a user
- Open `View my account`
- Verify profile values are displayed correctly

## Category Management

- Add a valid category
- Try adding category with existing ID
- Delete an unused category
- Try deleting category used by food

## Food Management

- Add a valid food with existing category
- Try adding food with duplicate ID
- Try adding food with missing category
- Update an existing food
- Delete an existing food
- Try deleting missing food

## Persistence

- Restart the application after registration and admin changes
- Verify stored accounts, categories, and foods remain available
- Verify repeated runs do not corrupt files
