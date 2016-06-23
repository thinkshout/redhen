<?php

namespace Drupal\redhen_contact\Plugin\Validation\Constraint;

use Drupal\Core\Access\AccessResult;
use Symfony\Component\Validator\Constraint;
use Symfony\Component\Validator\ConstraintValidator;

/**
 * Validates that a field is unique for the given entity type.
 */
class ContactEmailUniqueValidator extends ConstraintValidator {

  /**
   * {@inheritdoc}
   */
  public function validate($items, Constraint $constraint) {

    // Get Contact configuration.
    $config = \Drupal::config('redhen_contact.settings');

    // If mirroring Contacts to Drupal Users or config specifies unique email,
    // ensure email is unique.
    if ($config->get('connect_users') || $config->get('unique_email')) {
      $email = $this->context->getValue()->value;
      // Query to find out if email is taken.
      $email_taken = (bool) \Drupal::entityQuery('redhen_contact')
        ->condition('email', $email)
        ->execute();
      // Validation fails if email is already taken, succeeds otherwise.
      if ($email_taken) {
        $this->context->addViolation($constraint->message, [
          '%value' => $email,
        ]);
      }
    }
  }
}
