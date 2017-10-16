<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\redhen_connection\Entity\ConnectionType;

/**
 * Provides a form for deleting Connection entities.
 *
 * @ingroup redhen_connection
 */
class ConnectionDeleteForm extends ContentEntityDeleteForm {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete the @connection-type %name?', [
      '@connection-type' => ConnectionType::load($this->entity->bundle())->label(),
      '%name' => $this->entity->label()
    ]);
  }

}
