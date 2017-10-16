<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form controller for Connection edit forms.
 *
 * @ingroup redhen_connection
 */
class ConnectionForm extends ContentEntityForm {
  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form = parent::buildForm($form, $form_state);

    // If we have a fixed endpoint, we replace the input widget with the
    // rendered field.
    if ($fixed_endpoint = $form_state->get('fixed_endpoint')) {
      /* @var $connection \Drupal\redhen_connection\Entity\Connection */
      $connection = $this->getEntity();
      // @ TODO look into configuration of this display.
      $form[$fixed_endpoint] = $connection->{$fixed_endpoint}->view();
    }
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $entity = $this->entity;
    $status = parent::save($form, $form_state);

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created %label.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved %label.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.redhen_connection.canonical', ['redhen_connection' => $entity->id()]);
  }

}
