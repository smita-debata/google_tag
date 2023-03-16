<?php

declare(strict_types=1);

namespace Drupal\google_tag\Plugin\GoogleTag\Event;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\PluginFormInterface;

/**
 * Login event plugin.
 *
 * @GoogleTagEvent(
 *   id = "login",
 *   event_name = "login",
 *   label = @Translation("Login"),
 *   description = @Translation("Send this event to signify that a user has logged in."),
 *   dependency = "user"
 * )
 */
final class LoginEvent extends EventBase implements PluginFormInterface {

  /**
   * {@inheritDoc}
   */
  public function defaultConfiguration(): array {
    return [
      'method' => 'CMS',
    ];
  }

  /**
   * {@inheritDoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $form['method'] = [
      '#type' => 'textfield',
      '#title' => 'Login Method',
      '#default_value' => $this->configuration['method'],
      '#maxlength' => '254',
    ];
    return $form;
  }

  /**
   * {@inheritDoc}
   */
  public function validateConfigurationForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritDoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->configuration['method'] = $form_state->getValue('method');
  }

}
