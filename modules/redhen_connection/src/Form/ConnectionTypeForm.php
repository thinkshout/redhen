<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ConnectionTypeForm.
 *
 * @package Drupal\redhen_connection\Form
 */
class ConnectionTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    // Prepare the list of relatable entity types.
    $entity_types = $this->entityTypeManager->getDefinitions();
    $endpoint_entity_types = array_map(function ($entity_type) {
      return $entity_type->getLabel();
    }, $entity_types);

    $form['#tree'] = TRUE;
    /** @var \Drupal\redhen_connection\Entity\ConnectionType $redhen_connection_type */
    $redhen_connection_type = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_connection_type->label(),
      '#description' => $this->t("Label for the Connection type."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $redhen_connection_type->id(),
      '#machine_name' => [
        'exists' => '\Drupal\redhen_connection\Entity\ConnectionType::load',
      ],
      '#disabled' => !$redhen_connection_type->isNew(),
    ];

    $form['connection_label_pattern'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Connection label pattern'),
      '#maxlength' => 255,
      '#default_value' => $redhen_connection_type->get('connection_label_pattern'),
      '#description' => $this->t("Label pattern to use for connections. Use @label1 for the first entity and @label2 for the second."),
      '#required' => TRUE,
    ];

    // Set bundle specific settings for each of our endpoint fields.
    for ($x = 1; $x <= REDHEN_CONNECTION_ENDPOINTS; $x++) {
      $endpoint_type = $redhen_connection_type->getEndpointEntityTypeId($x);
      $form['endpoints'][$x] = [
        '#type' => 'fieldset',
        '#title' => $this->t('Endpoint @endpoint', ['@endpoint' => $x]),
      ];
      $form['endpoints'][$x]['entity_type'] = [
        '#type' => 'select',
        '#title' => $this->t('Entity type'),
        '#description' => $this->t('The entity type cannot be changes once created.'),
        '#default_value' => $endpoint_type,
        '#options' => $endpoint_entity_types,
        '#empty_value' => '',
        '#disabled' => !$redhen_connection_type->isNew(),
        '#ajax' => [
          'callback' => '::updateBundleOptions',
          'wrapper' => 'bundles-wrapper-' . $x,
        ]
      ];

      // Disable caching on this form.
      $form_state->setCached(FALSE);

      $form['endpoints'][$x]['label'] = [
        '#type' => 'textfield',
        '#title' => $this->t('Label'),
        '#description' => $this->t('If no label is provided, the entity type label will be used.'),
        '#default_value' => $redhen_connection_type->getEndpointLabel($x),
        '#empty_value' => '',
      ];

      $form['endpoints'][$x]['description'] = [
        '#type' => 'textarea',
        '#title' => $this->t('Description'),
        '#description' => $this->t('If no description is provided, the default field description will be used.'),
        '#default_value' => $redhen_connection_type->getEndpointDescription($x),
        '#empty_value' => '',
      ];

      $bundle_options = [];
      $endpoint_entity = (!empty($endpoint_type)) ? \Drupal::entityTypeManager()->getDefinition($endpoint_type) : FALSE;
      if ($endpoint_entity && $endpoint_entity->hasKey('bundle')) {
        $bundle_options = $this->getBundleOptions($endpoint_type);
      }

      $form['endpoints'][$x]['bundles'] = [
        '#type' => 'checkboxes',
        '#title' => $this->t('Bundles'),
        '#description' => $this->t('The allowed bundles for this endpoint.'),
        '#options' => $bundle_options,
        '#default_value' => (array) $redhen_connection_type->getEndpointBundles($x),
        '#required' => TRUE,
        '#size' => 6,
        '#multiple' => TRUE,
        '#prefix' => '<div id="bundles-wrapper-' . $x . '">',
        '#suffix' => '</div>',
        '#disabled' => !($endpoint_entity && $endpoint_entity->hasKey('bundle')),
      ];
    }


    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_connection_type = $this->entity;
    $status = $redhen_connection_type->save();

    switch ($status) {
      case SAVED_NEW:
        $this->messenger()->addMessage($this->t('Created the %label Connection type.', [
          '%label' => $redhen_connection_type->label(),
        ]));
        break;

      default:
        $this->messenger()->addMessage($this->t('Saved the %label Connection type.', [
          '%label' => $redhen_connection_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_connection_type->toUrl('collection'));
  }

  /**
   * Get the bundle options for a provided endpoint (entity) type.
   *
   * @param string $endpoint_type
   *
   * @return array
   */
  protected function getBundleOptions($endpoint_type) {
    $bundles = $this->entityManager->getBundleInfo($endpoint_type);
    $bundle_options = [];
    foreach ($bundles as $bundle_name => $bundle_info) {
      $bundle_options[$bundle_name] = $bundle_info['label'];
    }
    natsort($bundle_options);

    return $bundle_options;
  }

  /**
   * Ajax callback to update the bundle options.
   *
   * @param $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @return mixed
   */
  public function updateBundleOptions($form, FormStateInterface $form_state) {
    $element = $form_state->getTriggeringElement();
    $endpoint = $element['#array_parents'][1];
    $endpoint_type = $element['#value'];
    $form['endpoints'][$endpoint]['bundles']['#options'] = $this->getBundleOptions($endpoint_type);
    return $form['endpoints'][$endpoint]['bundles'];
  }

}
